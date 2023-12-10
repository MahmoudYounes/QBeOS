/***
 * kernel is the main entry point for the QBeOS
 * 
***/

#include "screen.h"
#include "memory.h"

void kmain()
{
    Screen screen = Screen();
    screen.ClearScreen();
    screen.WriteString("Initializing system...\n");

    Memory sysMemory = Memory(&screen);

    const char *message = "Welcome to QBeOS...\n";
    screen.WriteString(message);
    message = "QBeOS is just and educational OS, created by myounes just for fun.\n";
    screen.WriteString(message);

    sysMemory.PrintMemory(&screen);
    asm("hlt");
}
