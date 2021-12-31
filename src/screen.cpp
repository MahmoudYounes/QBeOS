/***
 * screen is the class responsible for managing write to screen operations
***/

#include "screen.h"

void Screen::ClearScreen()
{
    int charCounts = 0;
    volatile char* currCursorLocation = (char *)BaseMemoryAddress;
    while (charCounts < rowCount * colCount)
    {
        WriteCharacterToScreen(currCursorLocation, whiteSpace);
        currCursorLocation += 2;
        charCounts++;
    }
}

void Screen::WriteString(char* stringPtr)
{
    volatile char* currCursorLocation = (char *)BaseMemoryAddress;
    char* ptr = stringPtr;
    while (*ptr != '\0') {
        char ch = *ptr;
        short och = 0x0700 | ch;
        WriteCharacterToScreen(currCursorLocation, och);
        currCursorLocation += 2;
        ptr++;
    }
}

void Screen::WriteCharacterToScreen(volatile char* currLocation, const char characterToPrint)
{
    *(currLocation) = characterToPrint;
}