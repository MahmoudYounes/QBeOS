class Screen {
private:
    unsigned short* VideoMemory=(unsigned short*)0xb8000;    
    const short rowCount = 3;
    const short colCount = 9;
    const short format = 0x0002;
    const char whiteSpace = 0x0020;   
    int currCursorPos = 0;
public:
    void ClearScreen();

    void ScrollUp();

    void WriteString(char* stringPtr);

    void WriteCharacterToScreen(const char characterToPrint);
};