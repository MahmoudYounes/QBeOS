/***
 * kernel is the main entry point for the QBeOS
 * 
***/

#include "screen.h"
#include "memory.h"

extern Screen screen;
extern Memory sysMemory;

void kmain()
{
    screen = Screen();
    screen.ClearScreen();
    screen.WriteString("Initializing system...\n");

    sysMemory = Memory();

    const char *message = "Welcome to QBeOS...\n";
    screen.WriteString(message);
    message = "QBeOS is just and educational OS, created by myounes just for fun.\n";
    screen.WriteString(message);

    sysMemory.PrintMemory();
    sysMemory.PrintMemory();
    screen.WriteString("Hello, After scrolling\n");
    screen.WriteString("Hello, after scrolling2");
    asm("hlt");
}
