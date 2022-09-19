/***
 * screen is the class responsible for managing write to screen operations
***/

#include "screen.h"
#include "math.h"

/**
 * @brief fills the screen buffer with white spaces
 * 
 */
void Screen::ClearScreen()
{
    for(int i = 0; i < rowCount * colCount; i++) {
        VideoMemory[i]= format << 8 | whiteSpace;
    }
    currCursorPos = 0;
}

/**
 * @brief equivelent to printf except there is no f for now. f.
 * 
 * @param str 
 */
void Screen::WriteString(char* str)
{
    for(int i=0; str[i]!='\0';++i){
        WriteCharacterToScreen(str[i]);
    }
}

/**
 * @brief prints a character to screen. if buffer is full, scroll up. advances the currCursorPos
 * 
 * @param characterToPrint 
 */
void Screen::WriteCharacterToScreen(const char characterToPrint)
{
    if (currCursorPos == rowCount * colCount) {
        ScrollUp();
    }
    VideoMemory[currCursorPos] = format << 8 | characterToPrint;
    currCursorPos++;
}

/**
 * @brief Scrolls the screen up
 * This is a very ill implemented method that doesn't correctly scroll up except when the screen is full
 */
void Screen::ScrollUp() 
{
    if (currCursorPos <= colCount) {
        for (int j = 0; j < colCount;j++){
            VideoMemory[j] = format << 8 | whiteSpace;
        }
        currCursorPos = 0;
    }

    int i = 0, j = colCount;
    while (j < currCursorPos) {
        VideoMemory[i] = VideoMemory[j];
        i++;
        j++;
    }
    currCursorPos = Max(currCursorPos - colCount, 0);
}
