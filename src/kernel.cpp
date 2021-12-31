/***
 * kernel is the main entry point for the QBeOS
 * 
***/

#include "screen.h"

void kmain()
{
    Screen screen = Screen();
    char* string = "Hello, World!\0";
    screen.WriteString(string);
    asm("hlt");
}
