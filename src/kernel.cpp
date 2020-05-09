void InitializeTerminal();

void kmain()
{
    InitializeTerminal();
    *((int*)0xb8000)=0x07690748;
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
    while (charCounts < VD_ROW_COUNT * VD_COL_COUNT)
    {
        WriteCharacterToScreen(cursorLocation, WHITE_SPACE);
        cursorLocation++;
        cursorLocation++;
        charCounts++;
    }
}

void InitializeTerminal()
{
    ClearScreen();
}

void WriteString(char* stringPtr)
{

}