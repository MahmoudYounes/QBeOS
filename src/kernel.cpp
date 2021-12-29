void InitializeTerminal();
void WriteString(char* stringPtr);

void kmain()
{
    InitializeTerminal();
    char* string = "Hello, World!\0";
    WriteString(string);
    asm("hlt");
}

// base address for the video ram (VD => video default mode)
#define VD_BASE_MEM_ADDRESS 0xb8000
#define VD_COL_COUNT 80
#define VD_ROW_COUNT 25


// characters ASCII
#define WHITE_SPACE 0x0720

volatile char* cursorLocation = (char*)VD_BASE_MEM_ADDRESS;

void WriteCharacterToScreen(volatile char* currLocation, const char characterToPrint)
{
    *(currLocation) = characterToPrint;
}

void ClearScreen()
{
    int charCounts = 0;
    volatile char* currCursorLocation = cursorLocation;
    while (charCounts < VD_ROW_COUNT * VD_COL_COUNT)
    {
        WriteCharacterToScreen(currCursorLocation, WHITE_SPACE);
        currCursorLocation++;
        currCursorLocation++;
        charCounts++;
    }
}

void InitializeTerminal()
{
    ClearScreen();
}

void WriteString(char* stringPtr)
{
    volatile char* currCursorLocation = cursorLocation;
    char* ptr = stringPtr;
    while (*ptr != '\0') {
        char ch = *ptr;
        short och = 0x0700 | ch;
        WriteCharacterToScreen(currCursorLocation, och);
        currCursorLocation += 2;
        ptr++;
    }
}