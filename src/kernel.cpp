/***
 * kernel is the main entry point for the QBeOS
 * 
***/

#include "screen.h"
#include "memory.h"
#include "gdt.h"
#include "common.h"

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
    message = "QBeOS is just and educational OS, created by myounes just for fun.\n";
    screen.WriteString(message);

    sysMemory.PrintMemory();

    screen.WriteString("booting done...\n");
    screen.WriteString("halting PC...\n");
    HLT();
}
