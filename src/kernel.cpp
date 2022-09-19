/***
 * kernel is the main entry point for the QBeOS
 * 
***/

#include "screen.h"

void kmain()
{
    Screen screen = Screen();
    screen.ClearScreen();
    char* string1 = "123456789123456789123456789987654321\0";
    screen.WriteString(string1);
    screen.ScrollUp();
    char* string2 = "abcdefghijklmnopqrstuvwxyz\0";
    screen.WriteString(string2);
    asm("hlt");
}
