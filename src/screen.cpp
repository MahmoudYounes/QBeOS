/***
 * screen is the class responsible for managing write to screen operations
***/

#include "screen.h"

void Screen::ClearScreen()
{
    int charCounts = 0;
    volatile short* currCursorLocation = (short*)VideoMemory;
    for(int i = 0; i < rowCount * colCount; i++) {
        VideoMemory[i]=(VideoMemory[i] & 0x0200)|whiteSpace;
    }
}

void Screen::WriteString(char* str)
{
   for(int i=0; str[i]!='\0';++i){
        VideoMemory[i]=(VideoMemory[i] & 0xFF00)|str[i];
    }
}

void Screen::WriteCharacterToScreen(volatile short* currLocation, const char characterToPrint)
{
    *(currLocation) = characterToPrint;
}