/***
 * kernel is the main entry point for the QBeOS
 *
 ***/

// TODO: the includes in the project have pretty big names. need to reduce this
// naming. this requires modifying the build commands in the make file to work
// better with correct includes.

#include "acpi/include/acpi.h"
#include "arch/include/vmm.h"
#include "arch/x86/include/apic.h"
#include "arch/x86/include/cpuinfo.h"
#include "arch/x86/include/gdt.h"
#include "arch/x86/include/idt.h"
#include "arch/x86/include/mem_region.h"
#include "arch/x86/include/memory.h"
#include "arch/x86/include/pdt_entry.h"
#include "arch/x86/include/pic.h"
#include "arch/x86/include/processor.h"
#include "arch/x86/include/pt_entry.h"
#include "arch/x86/include/tss.h"
#include "arch/x86/include/pit.h"
#include "arch/x86/include/ps2.h"
#include "drivers/include/atkbd.h"
#include "drivers/include/driver.h"
#include "drivers/include/atkbd.h"
#include "include/common.h"
#include "include/logger.h"
#include "include/math.h"
#include "include/kargs.h"
#include "pci/include/pci.h"
#include "include/configs.h"

void kmain() __attribute__((noreturn));
void bootEnd() __attribute__((noreturn));

static char buf[1024];

// Global system vars.. only one should exist
extern Screen screen;
extern Memory sysMemory;
extern VirtualMemory vmm;
extern GDT gdt;
extern CPUInfo cpu;
extern IDT idt;
extern PIC pic;
extern APIC apic;
extern ACPIM acpi;
extern PCI pci;
extern PS2 ps2;
extern TSSManager tssManager;
extern PIT pit;

// For now it's easier for me to just look at the screen. I have a way in mind
// to automate this, so guess what... here is another TODO!
// TODO: automate testcases
// TODO: move unit tests to separate files
void testMemoryInitialization() {
  MemoryInfo memInfo = sysMemory.GetMemoryInfo();
  if (memInfo.memSizeBytes == 0) {
    kprint("FAILED: expected memory more than 0 GBs\0");
    panic("Memory Initialization tests failed\n\0");
  }

  kprint("Memory Initialization tests succeeded\n\0");
}

void testMemoryAllocation() {
  // while these allocations will succeed, writing to any of them will fail
  // sense these tests are run after paging is enabled. so they don't have
  // a vmm allocation counter part.
  uint64_t *memPtr = (uint64_t *)sysMemory.AllocPhysicalPage();
  kprint("allocated memory\n");

  kprint("freeing allocated page\n");
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

void testVMMAloocation() {
  char *memPtr = new char[100];
  char testBuf[] = "testing vmm allocation\n\0";
  memcpy(memPtr, testBuf, strlen(testBuf) * sizeof(char));

  uint8_t cmpRes = strcmp(memPtr, testBuf);
  if (cmpRes != 0) {
    kprint("FAILED: failed to write testBuf into allocated memory\n\0");
    panic("VMM Allocation tests failed\n\0");
  }
  delete[] memPtr;

  memPtr = new char[MB_TO_BYTE(10)];

  memset(memPtr, 'a', MB_TO_BYTE(10));

  for (uint64_t i = 0; i < MB_TO_BYTE(10); i++) {
    if (memPtr[i] != 'a') {
      kprint("FAILED: failed to write to allocated memory\n\0");
      panic("VMM Allocation tests failed\n\0");
    }
  }
  delete[] memPtr;

  kprint("VMM Allocation tests succeeded\n\0");
}

void testMemoryPageAt() {
  bool failed = false;
  MemoryRegion mem = sysMemory.GetPageAt(0x8000);
  if ((uintptr_t)mem.baseAddress != 0x8000) {
    kprintf(buf, "FAILED: expected %p found %p\n\0", 0x8000, mem.baseAddress);
    failed = true;
  }

  mem = sysMemory.GetPageAt(0x8020);
  if ((uintptr_t)mem.baseAddress != 0x8000) {
    kprintf(buf, "FAILED: expected %p found %p\n\0", 0x8000, mem.baseAddress);
    failed = true;
  }

  mem = sysMemory.GetPageAt(0x50000000);
  if ((uintptr_t)mem.baseAddress != 0x50000000) {
    kprintf(buf, "FAILED: expected %p found %p\n\0", 0x50000000,
            mem.baseAddress);
    failed = true;
  }

  if (failed) {
    panic("memory tests failed");
  }
  kprint("Memory PageAt tests succeeded\n\0");
}

void testFormater() {
  Formater formater = Formater();

  kprint("Testing formatter\n\0");

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

  // formater.Format(buf, "Multiform: %d %b %x %l\n\0", 123, 123, 123,
  // 123333333LL); screen.WriteString(buf); memset(buf, ' ', 512);

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
  for (int i = 0; i < 512; i++) {
    if (buf[i] != 'a') {
      panic("failed memset test\n\0");
    }
  }
  kprint("Memset tests succeeded\n\0");
}

void testPDTEntry() {
  uintptr_t pp = (uintptr_t)vmm.Allocate(PAGE_SIZE_BYTES);

  PDTEntry(MB)
      .SetIsUserAccessible()
      ->SetPageAttrTable()
      ->SetPTAddress((uint64_t)0xb00b55555)
      ->SetPresent()
      ->SetIsReadWrite()
      ->EncodeEntryAt(pp);

  uint32_t actual = *((uint32_t *)pp);
  uint32_t expectedEntry = 0b00000000100000010111000010000111;
  if (expectedEntry != actual) {
    kprint("FAILED:\0");
    kprintf(buf, " read after write value is %b expected %b \0",
            *((uint32_t *)pp), expectedEntry);
    panic("Test PDTEntry failed");
  }

  PDTEntry(KB)
      .SetPresent()
      ->SetIsReadWrite()
      ->SetGlobal() // should have no effect
      ->SetPTAddress((uint64_t)0xb00b55555)
      ->EncodeEntryAt(pp);

  actual = *((uint32_t *)pp);
  expectedEntry = 0b00000000101101010101000000000011;
  if (expectedEntry != actual) {
    kprint("FAILED:\0");
    kprintf(buf, " read after write value is %b expected %b \0",
            *((uint32_t *)pp), expectedEntry);
    panic("Test PDTEntry failed");
  }

  KERN_PDT->SetPTAddress(0x2501000)->EncodeEntryAt(pp);
  actual = *((uint32_t *)pp);
  expectedEntry = 0b10010100000001000000000011;
  if (expectedEntry != actual) {
    kprint("FAILED:\0");
    kprintf(buf, " read after write value is %b expected %b \0",
            *((uint32_t *)pp), expectedEntry);
    panic("Test PDTEntry failed");
  }

  USER_PDT->SetPTAddress(0x2501000)->EncodeEntryAt(pp);
  actual = *((uint32_t *)pp);
  expectedEntry = 0b10010100000001000000000111;
  if (expectedEntry != actual) {
    kprint("FAILED:\0");
    kprintf(buf, " read after write value is %b expected %b \0",
            *((uint32_t *)pp), expectedEntry);
    panic("Test PDTEntry failed");
  }

  vmm.Free((void *)pp);
  kprint("PDTEntry tests succeeded\n\0");
}

void testPTEntry() {
  uintptr_t pp = (uintptr_t)sysMemory.AllocPhysicalPage();

  PTEntry()
      .SetPresent()
      ->SetIsUserAccessible()
      ->SetGlobal()
      ->SetPageAddress(0xb00b5)
      ->EncodeEntryAt(pp);

  uint32_t actual = *((uint32_t *)pp);
  uint32_t expectedEntry = 0b10110000000100000101;
  if (expectedEntry != actual) {
    kprint("FAILED:\0");
    kprintf(buf, " read after write value is %b expected %b \0",
            *((uint32_t *)pp), expectedEntry);
    panic("Test PDTEntry failed");
  }

  PTEntry().SetPageAddress(0xfffff000)->EncodeEntryAt(pp);
  actual = *((uint32_t *)pp);
  expectedEntry = 0b11111111111111111111000000000000;
  if (expectedEntry != actual) {
    kprint("FAILED:\0");
    kprintf(buf, " read after write value is %b expected %b \0",
            *((uint32_t *)pp), expectedEntry);
    panic("Test PDTEntry failed");
  }

  KERN_PT->SetPageAddress(0x2501000)->EncodeEntryAt(pp);
  actual = *((uint32_t *)pp);
  expectedEntry = 0b10010100000001000000000011;
  if (expectedEntry != actual) {
    kprint("FAILED:\0");
    kprintf(buf, " read after write value is %b expected %b \0",
            *((uint32_t *)pp), expectedEntry);
    panic("Test PDTEntry failed");
  }

  USER_PT->SetPageAddress(0x2501000)->EncodeEntryAt(pp);
  actual = *((uint32_t *)pp);
  expectedEntry = 0b10010100000001000000000111;
  if (expectedEntry != actual) {
    kprint("FAILED:\0");
    kprintf(buf, " read after write value is %b expected %b \0",
            *((uint32_t *)pp), expectedEntry);
    panic("Test PDTEntry failed");
  }

  sysMemory.Free((void *)pp);
  kprint("PTEntry tests succeeded\n\0");
}

void testIDTEntry() {
  uintptr_t pg = (uintptr_t)vmm.Allocate(PAGE_SIZE_BYTES);

  IDTEntry idtEntry = IDTEntry();

  idtEntry.SetSegment(0x8); // segment 0x8
  idtEntry.SetOffset(0xff); // offset low 0xff offset high 0
  idtEntry.SetFlags(0x4e00);
  idtEntry.EncodeEntryAt(pg);

  uint64_t entry = *((uint64_t *)pg);
  IDTEntry actualEntry = IDTEntry(entry);
  if (actualEntry.GetFlags() != 0x4e00) {
    kprintf(buf, "Expected 0x4e00 found %x\n\0", actualEntry.GetFlags());
    panic("FAILED: IDTEntry tests\n\0");
  }

  if (actualEntry.GetSegment() != 0x8) {
    kprintf(buf, "Expected 0x8 found %x\n\0", actualEntry.GetSegment());
    panic("FAILED: IDTEntry tests\n\0");
  }

  if (actualEntry.GetOffset() != 0xff) {
    kprintf(buf, "Expected 0xff found %x\n\0", actualEntry.GetOffset());
    panic("FAILED: IDTEntry tests\n\0");
  }
}

// END TESTS

void setupConsole() {
  screen = Screen();
  screen.ClearScreen();
}

void printHelloMessage() {
  screen.ClearScreen();
  kprint("Welcome to QBeOS...\n\0");
}

void bootEnd() {
  // When we go to userspace we will not have this as system should always
  // be running until shutdown is specified.
  kprint("booting done...\n\0");
  kprint("halting PC...\n\0");
  HLT();
}

kargs *parseBootArgs(){
  uint32_t argsAddress;
  __asm__ ("mov %0, ebx"
    : "=b" (argsAddress)
    :);
   
  kargs *args = (kargs *)argsAddress;
  if (args->magic != BOOT_MAGIC){
    kprint("WARN: possibly incorrect boot header\n\0");
    // for some reason this check is broken even though the memory is set
    // correctly. kargs->magic is always zero. I am suspecting something
    // with alignment but don't know for sure.
    //return NULL;
  }
  return args;
}

void kmain() {
  // clearing the interrupts from BIOS because we havn't setup any interrupt
  // controller handler yet
  cli();
  setupConsole();

  kargs * args;
  char buf[255];
  args = parseBootArgs();
  if (args == NULL){
    panic("incorrect boot header\n\0");
  }

  kprint("booting kernel with the following args\n\0");
  kprintf(buf, "memRegionsCount: %d\n\0", args->memRegionsCount);
  kprintf(buf, "memTableStartAdrr: %p\n\0", args->memTableStartAddr);
  kprintf(buf, "pciSupported: %d\n\0", args->pciSupported);
  kprintf(buf, "pciConfigMechanism: %d\n\0", args->pciConfigMech);

  cpu = CPUInfo();
  // sys initializations
  kprint("Initializing all systems...\n\0");
  sysMemory = Memory(args);
  gdt = GDT();
  vmm = VirtualMemory(true /* should run vmm self tests before paging */);
  tssManager = TSSManager();
  idt = IDT();
  //pci = PCI(args);
  pit = PIT();
  ps2 = PS2();
  pic = PIC();
  //acpi = ACPIM();
  //apic = APIC(); 

  sti();
  pic.STI();

  pit.Reload();
  //Driver *kbd = new ATKBD(&ps2);
  //kbd->Initialize();  

  // at this point interrupts are disabled... need to setup IDT to renable them.

  bootEnd();
  // Systems initialized and we are booted yay!
  printHelloMessage();

  // Ideally this would be the init process running or a shell
  // Since I don't have a userspace env yet, this will be just
  // testing that systems are initialized and booted correctly.

  bootEnd();
}
