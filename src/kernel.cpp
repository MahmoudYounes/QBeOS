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

void kmain() __attribute__ ((noreturn));
void bootEnd() __attribute__ ((noreturn));

// Global system vars.. only one should exist
extern Screen screen;
extern Memory sysMemory;
extern VirtualMemory vmm;
extern GDT gdt;

// For now it's easier for me to just look at the screen. I have a way in mind
// to automate this, so guess what... here is another TODO!
// TODO: automate testcases
// TODO: move unit tests to separate files
void testMemoryInitialization(){
    sysMemory.PrintMemory();
}

void testMemoryAllocation(){
    screen.WriteString("allocating 1 page of physical memory.\n\0");
    uint64_t *memPtr = (uint64_t *)sysMemory.AllocPhysicalPage();
    screen.WriteString("allocated memory\n");

    screen.WriteString("freeing allocated page\n");
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

void testFormater(){
    Formater formater = Formater();
    char buf[FORMATER_BUFFER_SIZE_BYTES];

    screen.WriteString("Testing formatter\n\0");

    formater.Format(buf, "\n\ntwo new lines\n\0");
    screen.WriteString(buf);

    memset((uint8_t *)buf, ' ', FORMATER_BUFFER_SIZE_BYTES);

    formater.Format(buf, "num in decimal: %d\n\0", 123);
    screen.WriteString(buf);

    memset((uint8_t *)buf, ' ', FORMATER_BUFFER_SIZE_BYTES);

    formater.Format(buf, "%d formater at the begining\n\0", 123);
    screen.WriteString(buf);
    memset((uint8_t *)buf, ' ', FORMATER_BUFFER_SIZE_BYTES);

    formater.Format(buf, "num in bin: %b\n\0", 123);
    screen.WriteString(buf);
    memset((uint8_t *)buf, ' ', FORMATER_BUFFER_SIZE_BYTES);

    formater.Format(buf, "num in hex: %x\n\0", 123);
    screen.WriteString(buf);
    memset((uint8_t *)buf, ' ', FORMATER_BUFFER_SIZE_BYTES);

    formater.Format(buf, "num in long: %l\n\0", 123333333333LL);
    screen.WriteString(buf);
    memset((uint8_t *)buf, ' ', FORMATER_BUFFER_SIZE_BYTES);

    formater.Format(buf, "Multiform: %d %b %x %l\n\0", 123, 123, 123, 123333333LL);
    screen.WriteString(buf);

    memset((uint8_t *)buf, ' ', FORMATER_BUFFER_SIZE_BYTES);

    // FIXME: a bug here, printing x x x and x makes last x output zero
    uintptr_t p1 = 123;
    uintptr_t p2 = 123;
    uint64_t p3 = 123;
    uintptr_t p4 = 123;
    formater.Format(buf, "four xs: %x %x %x %x\n\0", p1, p2, p3, p4);
    screen.WriteString(buf);
    memset((uint8_t *)buf, ' ', FORMATER_BUFFER_SIZE_BYTES);

    screen.WriteString("Done testing formatter\n\0");
}

void testMemset() {
    char buf[FORMATER_BUFFER_SIZE_BYTES];
    memset(buf, 'a', FORMATER_BUFFER_SIZE_BYTES);
    for (int i = 0; i < FORMATER_BUFFER_SIZE_BYTES; i++){
        if (buf[i] != 'a'){
            screen.WriteString("expected: 'a' found: \0");
            screen.WriteInt(buf[i]);
            screen.WriteString("\n\0");
            panic("failed memset test");
        }
    }
    screen.WriteString("memset tests passed\n\0");
}

void testPDTEntry() {
    screen.WriteString("testing PDTEntry\n\0");
    char *buf = (char *) sysMemory.AllocPhysicalPage();
    uintptr_t pp = (uintptr_t)sysMemory.AllocPhysicalPage();

    PDTEntry(MB).SetIsUserAccessible()
        ->SetPageAttrTable()
        ->SetPTAddress((uint64_t)0xb00b55555)
        ->SetPresent()
        ->SetIsReadWrite()
        ->EncodeEntryAt(pp);

    uint32_t actual = *((uint32_t *)pp);
    uint32_t expectedEntry = 0b00000000100000010111000010000111;
    const char *verdict = expectedEntry == actual? "OK\n\0":"FAILED\n\0";
    Formater().Format(buf, "read after write value is %b expected %b \0", *((uint32_t*)pp), expectedEntry);
    screen.WriteString(buf);
    screen.WriteString(verdict);

    memset((uint8_t *)buf, ' ', 4096);
    memset((uint8_t *)pp, ' ', 4096);

    PDTEntry(KB).SetPresent()
        ->SetIsReadWrite()
        ->SetGlobal() // should have no effect
        ->SetPTAddress((uint64_t) 0xb00b55555)
        ->EncodeEntryAt(pp);

    actual = *((uint32_t *)pp);
    expectedEntry = 0b00000000101101010101000000000011;
    verdict = expectedEntry == actual? "OK\n\0":"FAILED\n\0";
    Formater().Format(buf, "read after write value is %b expected %b \0", *((uint32_t*)pp), expectedEntry);
    screen.WriteString(buf);
    screen.WriteString(verdict);

    memset((uint8_t *)buf, ' ', 4096);
    memset((uint8_t *)pp, ' ', 4096);

    KERN_PDT->SetPTAddress(0x2501000)->EncodeEntryAt(pp);
    actual = *((uint32_t *)pp);
    expectedEntry = 0b10010100000001000000000011;
    verdict = expectedEntry == actual? "OK\n\0":"FAILED\n\0";
    Formater().Format(buf, "read after write value is %b expected %b \0", *((uint32_t*)pp), expectedEntry);
    screen.WriteString(buf);
    screen.WriteString(verdict);

    memset((uint8_t *)buf, ' ', 4096);
    memset((uint8_t *)pp, ' ', 4096);

    USER_PDT->SetPTAddress(0x2501000)->EncodeEntryAt(pp);
    actual = *((uint32_t *)pp);
    expectedEntry = 0b10010100000001000000000111;
    verdict = expectedEntry == actual? "OK\n\0":"FAILED\n\0";
    Formater().Format(buf, "read after write value is %b expected %b \0", *((uint32_t*)pp), expectedEntry);
    screen.WriteString(buf);
    screen.WriteString(verdict);

    memset((uint8_t *)buf, ' ', 4096);
    memset((uint8_t *)pp, ' ', 4096);

    sysMemory.Free((void *)pp);
    sysMemory.Free((void *)buf);
}

void testPTEntry(){
    screen.WriteString("testing PTEntry\n\0");
    char *buf = (char *) sysMemory.AllocPhysicalPage();
    uintptr_t pp = (uintptr_t)sysMemory.AllocPhysicalPage();

    PTEntry().SetPresent()
        ->SetIsUserAccessible()
        ->SetGlobal()
        ->SetPageAddress(0xb00b5)
        ->EncodeEntryAt(pp);

    uint32_t actual = *((uint32_t *)pp);
    uint32_t expectedEntry = 0b10110000000100000101;
    const char *verdict = expectedEntry == actual? "OK\n\0":"FAILED\n\0";
    Formater().Format(buf, "read after write value is %b expected %b \0", *((uint32_t*)pp), expectedEntry);
    screen.WriteString(buf);
    screen.WriteString(verdict);

    memset((uint8_t *)buf, ' ', 4096);
    memset((uint8_t *)pp, ' ', 4096);

    PTEntry().SetPageAddress(0xfffff000)->EncodeEntryAt(pp);

    actual = *((uint32_t *)pp);
    expectedEntry = 0b11111111111111111111000000000000;
    verdict = expectedEntry == actual? "OK\n\0":"FAILED\n\0";
    Formater().Format(buf, "read after write value is %b expected %b \0", *((uint32_t*)pp), expectedEntry);
    screen.WriteString(buf);
    screen.WriteString(verdict);

    memset((uint8_t *)buf, ' ', 4096);
    memset((uint8_t *)pp, ' ', 4096);

    KERN_PT->SetPageAddress(0x2501000)->EncodeEntryAt(pp);
    actual = *((uint32_t *)pp);
    expectedEntry = 0b10010100000001000000000011;
    verdict = expectedEntry == actual? "OK\n\0":"FAILED\n\0";
    Formater().Format(buf, "read after write value is %b expected %b \0", *((uint32_t*)pp), expectedEntry);
    screen.WriteString(buf);
    screen.WriteString(verdict);

    memset((uint8_t *)buf, ' ', 4096);
    memset((uint8_t *)pp, ' ', 4096);

    USER_PT->SetPageAddress(0x2501000)->EncodeEntryAt(pp);
    actual = *((uint32_t *)pp);
    expectedEntry = 0b10010100000001000000000111;
    verdict = expectedEntry == actual? "OK\n\0":"FAILED\n\0";
    Formater().Format(buf, "read after write value is %b expected %b \0", *((uint32_t*)pp), expectedEntry);
    screen.WriteString(buf);
    screen.WriteString(verdict);

    memset((uint8_t *)buf, ' ', 4096);
    memset((uint8_t *)pp, ' ', 4096);

    sysMemory.Free((void *)pp);
    sysMemory.Free((void *)buf);
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
    //testMemoryInitialization();
    //testMemoryAllocation();
    //testFormater();
    //testPDTEntry();
    //testPTEntry();
    //testMemset();

    bootEnd();
}
