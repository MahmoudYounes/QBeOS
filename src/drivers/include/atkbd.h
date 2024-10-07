#ifndef KBD_H
#define KBD_H

#include "include/common.h"
#include "include/logger.h"
#include "arch/x86/include/ps2.h"
#include "drivers/include/driver.h"
#include "arch/x86/include/pic.h"

#define ENABLE 0xf4
#define RESET 0xff
#define ID 0xf2

extern PIC pic;

class ATKBD : public Driver{
  PS2 *psc;

  public:
  ATKBD(PS2 *psc);
  void Initialize();
  void Handler();
};

#endif
