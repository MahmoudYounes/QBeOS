#include "arch/x86/include/pit.h"

PIT::PIT(){}

PIT::PIT(PIC *pic){
  this->pic = pic;
  uint8_t cmd = 0;

  cmd = CHANL0 | LOHIB | TERMC | BIN;
  outb(CMDCH, cmd);

  pic->DisableInterrupt(0);
}

void PIT::Reload(){
  // we only want to program channel 0 for now.
  // that is the channel connected to IRQ0 in PIC.
  //
  uint8_t lobyte, hibyte;
  uint16_t divisor;

  divisor = DIVISOR;
 
  lobyte = divisor & 0xff;
  hibyte = (divisor & 0xff00) >> 8;
  outb(CHANL0, lobyte);
  outb(CHANL0, hibyte);
}
