#include "arch/x86/include/pit.h"

PIT::PIT(){
  // we only want to program channel 0 for now.
  // that is the channel connected to IRQ0 in PIC.
  //
}

void PIT::Reload(){
  uint8_t cmd = 0, lobyte, hibyte;
  uint16_t divisor;

  cmd = CHANL0 | LOHIB | TERMC | BIN;
  outb(CMDCH, cmd);

  divisor = DIVISOR;
 
  lobyte = divisor & 0xff;
  hibyte = (divisor & 0xff00) >> 8;
  outb(CHANL0, lobyte);
  outb(CHANL0, hibyte);
}
