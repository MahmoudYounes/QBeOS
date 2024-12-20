#ifndef KBD_H
#define KBD_H

#include "include/common.h"
#include "include/logger.h"
#include "arch/x86/include/ps2.h"
#include "drivers/include/driver.h"
#include "arch/x86/include/pic.h"
#include "arch/x86/include/interrupt_32.h"
#include "kstdlib/include/vector.h"


#define ENABLE 0xf4
#define RESET 0xff
#define GET_ID 0xf2
#define SET_LEDS 0xed
#define MOD_SCANSET 0xf0
#define RESEND_LST_BYTE 0xfe

DEFINE_INTERRUPT(KeyboardHandler);

class ATKBD : public Driver{
private:
  PIC *pic;
  PS2 *psc;

  uint8_t id[2];
  uint8_t ledStatus;

  // store the queue of characters?  
  Vector<char> charQueue;

  void printKeyboardStatus();
  void idkbd();
  void setScanSet();
  void updateLeds();
  void handleData(uint8_t data);
public:
  ATKBD();
  ATKBD(PIC *pic, PS2 *psc);
  void Initialize();
  void Handler();
};

inline ATKBD kbdDriver;


#endif
