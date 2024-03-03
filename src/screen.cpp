#include "include/screen.h"

Screen::Screen() {
  // disable the cursor
  outb(0x3d4, 0x0a);
  outb(0x3d5, 0x20);
}

void Screen::ClearScreen() {
  for (int i = 0; i < rowCount * colCount; i++) {
    VideoMemory[i] = format << 8 | whiteSpace;
  }
  currCursorPos = 0;
}

void Screen::WriteString(const char *str) {
  for (int i = 0; str[i] != '\0'; ++i) {
    WriteCharacterToScreen(str[i]);
  }

  if (_Ddebug) {
    _Ddebug = false;
    WriteString("currCursorPos is ");
    WriteInt(currCursorPos);
    WriteString("\n");
    _Ddebug = true;
  }
}

void Screen::WriteCharacterToScreen(const char characterToPrint) {
  if (currCursorPos == rowCount * colCount) {
    ScrollUp();
  }
  if (characterToPrint == '\n') {
    int currRow = currCursorPos / colCount;
    currCursorPos = (currRow + 1) * colCount;
    // TODO: Handle scrollup here
    return;
  }
  VideoMemory[currCursorPos] = format << 8 | characterToPrint;
  currCursorPos++;
}

void Screen::ScrollUp() {
  // scroll one line up
  int i = 0, j = colCount;
  while (j < currCursorPos) {
    VideoMemory[i] = VideoMemory[j];
    i++;
    j++;
  }

  while (i < currCursorPos) {
    VideoMemory[i++] = format << 8 | whiteSpace;
  }

  currCursorPos = Max(currCursorPos - colCount, 0);
}

void Screen::WriteInt(const uint64_t numToPrint) {
  uint64_t num = numToPrint;
  char bf[10];
  int li = 9;
  if (num == 0) {
    WriteCharacterToScreen('0');
    return;
  }
  for (; num > 0;) {
    int res = num % 10;
    num = num / 10;
    bf[li] = '0' + res;
    li--;
  }
  for (li++; li < 10; li++) {
    WriteCharacterToScreen(bf[li]);
  }
}

void Screen::_DenableSelfDebug() { _Ddebug = true; }

Screen screen;
