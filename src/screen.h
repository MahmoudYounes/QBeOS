class Screen {
private:
    const volatile char BaseMemoryAddress = 0xb8000;
    const int rowCount = 25;
    const int colCount = 80;
    const char format = 0x07;
    const char whiteSpace = 0x20;
    
    int currCursorPos = 0;
public:
    void ClearScreen();

    void WriteString(char* stringPtr);

    void WriteCharacterToScreen(volatile char* currLocation, const char characterToPrint);
};