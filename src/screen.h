class Screen {
private:
    unsigned short* VideoMemory=(unsigned short*)0xb8000;    
    const short rowCount = 24;
    const short colCount = 80;
    const short format = 0x0002;
    const char whiteSpace = 0x0020;   
    int currCursorPos = 0;
public:
    void ClearScreen();

    void ScrollUp();

    void WriteString(char* stringPtr);

    void WriteCharacterToScreen(const char characterToPrint);
};