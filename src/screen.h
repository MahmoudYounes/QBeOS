class Screen {
private:
    unsigned short* VideoMemory=(unsigned short*)0xb8000;    
    const int rowCount = 25;
    const int colCount = 80;
    const char format = 0xFF00;
    const char whiteSpace = 0x0020;   
    int currCursorPos = 0;
public:
    void ClearScreen();

    void WriteString(char* stringPtr);

    void WriteCharacterToScreen(volatile short* currLocation, const char characterToPrint);
};