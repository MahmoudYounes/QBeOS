#ifndef KBD_H
#define KBD_H

#include "include/common.h"
#include "include/logger.h"
#include "arch/x86/include/ps2.h"
#include "drivers/include/driver.h"
#include "arch/x86/include/pic.h"
#include "arch/x86/include/interrupt_32.h"

#define ENABLE 0xf4
#define RESET 0xff
#define ID 0xf2
#define SET_LEDS 0xed

DEFINE_INTERRUPT(KeyboardHandler);

class ATKBD : public Driver{
private:
  PIC *pic;
  PS2 *psc;
  void printKeyboardStatus();
  void enableLeds();
  void handleData(uint8_t data);
public:
  ATKBD();
  ATKBD(PIC *pic, PS2 *psc);
  void Initialize();
  void Handler();
};

inline ATKBD kbdDriver;


#endif
