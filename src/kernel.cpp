/***
 * kernel is the main entry point for the QBeOS
 * 
***/

#include "mem_region.h"
#include "pdt_entry.h"
#include "pt_entry.h"
#include "vmm.h"
#include "screen.h"
#include "memory.h"
#include "gdt.h"
#include "common.h"
#include "math.h"
#include "formater.h"
#include "cpuinfo.h"

void kmain() __attribute__ ((noreturn));
void bootEnd() __attribute__ ((noreturn));

static char buf[1024];

// Global system vars.. only one should exist
extern Screen screen;
extern Memory sysMemory;
extern VirtualMemory vmm;
extern GDT gdt;
extern CPUInfo cpu;

// For now it's easier for me to just look at the screen. I have a way in mind
// to automate this, so guess what... here is another TODO!
// TODO: automate testcases
// TODO: move unit tests to separate files
void testMemoryInitialization(){
    MemoryInfo memInfo = sysMemory.GetMemoryInfo();
    if (memInfo.memSizeBytes == 0){
        print("FAILED: expected memory more than 0 GBs\0");
        panic("Memory Initialization tests failed\n\0");
    }

    print("Memory Initialization tests succeeded\n\0");
}

void testMemoryAllocation(){
    // while these allocations will succeed, writing to any of them will fail
    // sense these tests are run after paging is enabled. so they don't have
    // a vmm allocation counter part.
    uint64_t *memPtr = (uint64_t *)sysMemory.AllocPhysicalPage();
    print("allocated memory\n");

    print("freeing allocated page\n");
    sysMemory.Free(memPtr);

    screen.WriteString("allocating 16KBs i.e 4 Pages of memory\n\0");
    memPtr = (uint64_t *)sysMemory.Allocate(16 << 10);
    screen.WriteString("allocated memory\n\0");
    sysMemory.PrintMemory();

    screen.WriteString("freeing memory\n\0");
    sysMemory.Free(memPtr);
    screen.WriteString("freed memory\n\0");
    sysMemory.PrintMemory();
}

void testVMMAloocation(){
    char *memPtr = (char *)vmm.Allocate(100);
    char testBuf[] = "testing vmm allocation\n\0";
    memcpy(memPtr, testBuf, strlen(testBuf)*sizeof(char));

    uint8_t cmpRes = strcmp(memPtr, testBuf);
    if (cmpRes != 0){
        print("FAILED: failed to write testBuf into allocated memory\n\0");
        panic("VMM Allocation tests failed\n\0");
    }
    vmm.Free(memPtr);

    memPtr = (char *)vmm.Allocate(MB_TO_BYTE(10));

    memset(memPtr, 'a', MB_TO_BYTE(10));

    for (uint64_t i = 0; i < MB_TO_BYTE(10);i++){
        if(memPtr[i] != 'a'){
            print("FAILED: failed to write to allocated memory\n\0");
            panic("VMM Allocation tests failed\n\0");
        }
    }
    vmm.Free(memPtr);

    print("VMM Allocation tests succeeded\n\0");
}

void testMemoryPageAt(){
    bool failed = false;
    MemoryRegion mem = sysMemory.GetPageAt(0x8000);
    if ((uintptr_t)mem.baseAddress != 0x8000){
        printf(buf, "FAILED: expected %p found %p\n\0", 0x8000, mem.baseAddress);
        failed = true;
    }

    mem = sysMemory.GetPageAt(0x8020);
    if ((uintptr_t)mem.baseAddress != 0x8000){
        printf(buf, "FAILED: expected %p found %p\n\0", 0x8000, mem.baseAddress);
        failed = true;
    }

    mem = sysMemory.GetPageAt(0x50000000);
    if ((uintptr_t)mem.baseAddress != 0x50000000){
        printf(buf, "FAILED: expected %p found %p\n\0", 0x50000000, mem.baseAddress);
        failed = true;
    }

    if (failed){
        panic("memory tests failed");
    }
    print("Memory PageAt tests succeeded\n\0");
}

void testFormater(){
    Formater formater = Formater();

    print("Testing formatter\n\0");

    formater.Format(buf, "\n\ntwo new lines\n\0");
    screen.WriteString(buf);

    memset(buf, ' ', 512);

    formater.Format(buf, "num in decimal: %d\n\0", 123);
    screen.WriteString(buf);

    memset(buf, ' ', 512);

    formater.Format(buf, "%d formater at the begining\n\0", 123);
    screen.WriteString(buf);
    memset(buf, ' ', 512);

    formater.Format(buf, "num in bin: %b\n\0", 123);
    screen.WriteString(buf);
    memset(buf, ' ', 512);

    formater.Format(buf, "num in hex: %x\n\0", 123);
    screen.WriteString(buf);
    memset(buf, ' ', 512);

    formater.Format(buf, "num in long: %l\n\0", 123333333333LL);
    screen.WriteString(buf);
    memset(buf, ' ', 512);

    // FIXME: a bug here, when paging is enabled.

    // formater.Format(buf, "Multiform: %d %b %x %l\n\0", 123, 123, 123, 123333333LL);
    // screen.WriteString(buf);
    // memset(buf, ' ', 512);

    // uint64_t p1 = 123;
    // uint64_t p2 = 123;
    // uint64_t p3 = 123;
    // uint64_t p4 = 123;
    // formater.Format(buf, "four xs: %x %x %x %x\n\0", p1, p2, p3, p4);
    // screen.WriteString(buf);
    // memset(buf, ' ', 512);

    screen.WriteString("Done testing formatter\n\0");
}

void testMemset() {
    memset(buf, 'a', 512);
    for (int i = 0; i < 512; i++){
        if (buf[i] != 'a'){
            panic("failed memset test\n\0");
        }
    }
    print("Memset tests succeeded\n\0");
}

void testPDTEntry() {
    uintptr_t pp = (uintptr_t)vmm.Allocate(PHYSICAL_PAGE_SIZE);

    PDTEntry(MB).SetIsUserAccessible()
        ->SetPageAttrTable()
        ->SetPTAddress((uint64_t)0xb00b55555)
        ->SetPresent()
        ->SetIsReadWrite()
        ->EncodeEntryAt(pp);

    uint32_t actual = *((uint32_t *)pp);
    uint32_t expectedEntry = 0b00000000100000010111000010000111;
    if (expectedEntry != actual){
        print("FAILED:\0");
        printf(buf, " read after write value is %b expected %b \0", *((uint32_t*)pp), expectedEntry);
        panic("Test PDTEntry failed");
    }

    PDTEntry(KB).SetPresent()
        ->SetIsReadWrite()
        ->SetGlobal() // should have no effect
        ->SetPTAddress((uint64_t) 0xb00b55555)
        ->EncodeEntryAt(pp);

    actual = *((uint32_t *)pp);
    expectedEntry = 0b00000000101101010101000000000011;
    if (expectedEntry != actual){
        print("FAILED:\0");
        printf(buf, " read after write value is %b expected %b \0", *((uint32_t*)pp), expectedEntry);
        panic("Test PDTEntry failed");
    }

    KERN_PDT->SetPTAddress(0x2501000)->EncodeEntryAt(pp);
    actual = *((uint32_t *)pp);
    expectedEntry = 0b10010100000001000000000011;
    if (expectedEntry != actual){
        print("FAILED:\0");
        printf(buf, " read after write value is %b expected %b \0", *((uint32_t*)pp), expectedEntry);
        panic("Test PDTEntry failed");
    }

    USER_PDT->SetPTAddress(0x2501000)->EncodeEntryAt(pp);
    actual = *((uint32_t *)pp);
    expectedEntry = 0b10010100000001000000000111;
    if (expectedEntry != actual){
        print("FAILED:\0");
        printf(buf, " read after write value is %b expected %b \0", *((uint32_t*)pp), expectedEntry);
        panic("Test PDTEntry failed");
    }

    vmm.Free((void *)pp);
    print("PDTEntry tests succeeded\n\0");
}

void testPTEntry(){
    uintptr_t pp = (uintptr_t)sysMemory.AllocPhysicalPage();

    PTEntry().SetPresent()
        ->SetIsUserAccessible()
        ->SetGlobal()
        ->SetPageAddress(0xb00b5)
        ->EncodeEntryAt(pp);

    uint32_t actual = *((uint32_t *)pp);
    uint32_t expectedEntry = 0b10110000000100000101;
    if (expectedEntry != actual){
        print("FAILED:\0");
        printf(buf, " read after write value is %b expected %b \0", *((uint32_t*)pp), expectedEntry);
        panic("Test PDTEntry failed");
    }

    PTEntry().SetPageAddress(0xfffff000)->EncodeEntryAt(pp);
    actual = *((uint32_t *)pp);
    expectedEntry = 0b11111111111111111111000000000000;
    if (expectedEntry != actual){
        print("FAILED:\0");
        printf(buf, " read after write value is %b expected %b \0", *((uint32_t*)pp), expectedEntry);
        panic("Test PDTEntry failed");
    }

    KERN_PT->SetPageAddress(0x2501000)->EncodeEntryAt(pp);
    actual = *((uint32_t *)pp);
    expectedEntry = 0b10010100000001000000000011;
    if (expectedEntry != actual){
        print("FAILED:\0");
        printf(buf, " read after write value is %b expected %b \0", *((uint32_t*)pp), expectedEntry);
        panic("Test PDTEntry failed");
    }

    USER_PT->SetPageAddress(0x2501000)->EncodeEntryAt(pp);
    actual = *((uint32_t *)pp);
    expectedEntry = 0b10010100000001000000000111;
    if (expectedEntry != actual){
        print("FAILED:\0");
        printf(buf, " read after write value is %b expected %b \0", *((uint32_t*)pp), expectedEntry);
        panic("Test PDTEntry failed");
    }

    sysMemory.Free((void *)pp);
    print("PTEntry tests succeeded\n\0");
}

void setupConsole(){
    screen = Screen();
    screen.ClearScreen();
}

void printHelloMessage(){
    screen.ClearScreen();
    screen.WriteString("Welcome to QBeOS...\n\0");
}

void bootEnd(){
    // When we go to userspace we will not have this as system should always
    // be running until shutdown is specified.
    screen.WriteString("booting done...\n\0");
    screen.WriteString("halting PC...\n\0");
    HLT();
}

void kmain() {
    cli();
    setupConsole();
    cpu = CPUInfo();

    // sys initializations
    screen.WriteString("Initializing all systems...\n\0");
    sysMemory = Memory();
    gdt = GDT();
    vmm = VirtualMemory(false /* should run vmm self tests before paging */);

    // at this point interrupts are disabled... need to setup IDT to renable them.

    // Systems initialized and we are booted yay!
    printHelloMessage();

    // Ideally this would be the init process running or a shell
    // Since I don't have a userspace env yet, this will be just
    // testing that systems are initialized and booted correctly.

    screen.WriteString("Running self tests\n\0");
    testMemoryInitialization();
    testMemoryAllocation();
    testMemoryPageAt();
    // these tests will not page until vmm.alloc is implemented
    //testPDTEntry();
    //testPTEntry();
    //testMemset();
    //testFormater();
    testVMMAloocation();

    bootEnd();
}
