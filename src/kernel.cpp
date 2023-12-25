/***
 * kernel is the main entry point for the QBeOS
 * 
***/

#include "mem_region.h"
#include "screen.h"
#include "memory.h"
#include "gdt.h"
#include "common.h"
#include "math.h"

void kmain() __attribute__ ((noreturn));

extern Screen screen;
extern Memory sysMemory;
extern GDT gdt;

void kmain() {
    cli();
    screen = Screen();
    screen.ClearScreen();
    screen.WriteString("Initializing system...\n");

    sysMemory = Memory();
    gdt = GDT();

    // at this point interrupts are disabled... need to setup IDT to renable them.
    const char *message = "Welcome to QBeOS...\n";
    screen.WriteString(message);

    sysMemory.PrintMemory();

    screen.WriteString("allocating 1 page of physical memory.\n");
    uint64_t *memPtr = (uint64_t *)sysMemory.AllocPhysicalPage();
    screen.WriteString("allocated memory\n");

    screen.WriteString("freeing allocated page\n");
    sysMemory.Free(memPtr);

    screen.WriteString("allocating 16KBs i.e 4 Pages of memory\n");
    memPtr = (uint64_t *)sysMemory.Allocate(16 << 10);
    screen.WriteString("allocated memory\n");
    sysMemory.PrintMemory();

    screen.WriteString("freeing memory\n");
    sysMemory.Free(memPtr);
    screen.WriteString("freed memory\n");
    sysMemory.PrintMemory();

    screen.WriteString("booting done...\n");
    screen.WriteString("halting PC...\n");
    HLT();
}
