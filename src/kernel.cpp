/***
 * kernel is the main entry point for the QBeOS
 * 
***/

#include "screen.h"

void kmain()
{
    Screen screen = Screen();
    screen.ClearScreen();
    screen._DenableSelfDebug();

    const char *message = "Welcome to QBeOS...";
    screen.WriteString(message);

    message = "QBeOS is just and educational OS, created by myounes just for fun.";
    screen.WriteString(message);

    asm("hlt");
}
