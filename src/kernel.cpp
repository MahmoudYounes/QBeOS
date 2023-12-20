/***
 * kernel is the main entry point for the QBeOS
 * 
***/

#include "screen.h"
#include "memory.h"
#include "gdt.h"

extern Screen screen;
extern Memory sysMemory;
extern GDT gdt;

void kmain()
{
    screen = Screen();
    screen.ClearScreen();
    screen.WriteString("Initializing system...\n");

    sysMemory = Memory();
    gdt = GDT();

    const char *message = "Welcome to QBeOS...\n";
    screen.WriteString(message);
    message = "QBeOS is just and educational OS, created by myounes just for fun.\n";
    screen.WriteString(message);

    sysMemory.PrintMemory();

    screen.WriteString("booting done...halting...\n");
    asm("hlt");
}
