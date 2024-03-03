#ifndef SCREEN_H
#define SCREEN_H

#include "include/common.h"
#include "include/math.h"

class Screen {
private:
  // VideoMemory is the address of the video memory to write to
  uint16_t *VideoMemory = (uint16_t *)0xb8000;

  // Screen management vars
  static const short rowCount = 24;
  static const short colCount = 80;
  static const short format = 0x2;
  static const char whiteSpace = 0x0020;
  int currCursorPos = 0;

  // debug enables using this library to debug it self
  bool _Ddebug = false;
  int _DcallCounts = 0;

public:
  Screen();

  void ClearScreen();

  void ScrollUp();

  void WriteString(const char *stringPtr);

  void WriteCharacterToScreen(const char characterToPrint);

  void WriteInt(const uint64_t numToPrint);

  void _DenableSelfDebug();
};

#endif /* SCREEN_H */
