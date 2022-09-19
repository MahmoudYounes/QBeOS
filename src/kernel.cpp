/***
 * kernel is the main entry point for the QBeOS
 * 
***/

#include "screen.h"

void kmain()
{
    Screen screen = Screen();
    screen.ClearScreen();
    char* string = "Hello, World!\0";
    screen.WriteString(string);
    asm("hlt");
}
