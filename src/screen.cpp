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
void Screen::WriteString(const char *str)
{
    for(int i=0; str[i]!='\0';++i){
        WriteCharacterToScreen(str[i]);
    }

    if (_Ddebug) {
        _Ddebug = false;
        WriteString("currCursorPos is ");
        WriteIntToScreen(currCursorPos);
        WriteString("\n");
        _Ddebug = true;

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
    if (characterToPrint == '\n'){
        int currRow = currCursorPos / colCount;
        currCursorPos = (currRow + 1) * colCount;
        // TODO: Handle scrollup here
        return;
    }
    VideoMemory[currCursorPos] = format << 8 | characterToPrint;
    currCursorPos++;
}

/**
 * @brief Scrolls the screen up
 * This is a very ill implemented method that doesn't correctly scroll up in the standard expected way
 * this will be more imporved when input from user is added
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

/**
 * @brief prints an integer number to screen
 * 
 * @param num 
 */
void Screen::WriteIntToScreen(int num) {
    char bf[10];
    int li = 9;
    if (num == 0){
        WriteCharacterToScreen('0');
        return;
    }
    for (;num > 0;) {
        int res = num % 10;
        num = num / 10;
        bf[li] = '0' + res;
        li--;
    }
    for (li++;li < 10; li++){
        WriteCharacterToScreen(bf[li]);
    }
}


void Screen::_DenableSelfDebug(){
    _Ddebug = true;
}
