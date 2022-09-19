/***
 * kernel is the main entry point for the QBeOS
 * 
***/

#include "screen.h"

void kmain()
{
    Screen screen = Screen();
    screen.ClearScreen();
    screen.WriteString("I Love You, Queen Be!\n\0");
    char* string1 = "12345\0";
    screen.WriteString(string1);
    //screen.ScrollUp();
    char* string2 = "abcdefghijklmnopqrstuvwxyz\0";
    screen.WriteString(string2);
    screen.WriteIntToScreen(2147483647);
    screen.WriteString("\n\0");
    screen.WriteString("Hello,\nWorld!\0");
    asm("hlt");
}
