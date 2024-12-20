#include "drivers/include/atkbd.h"
#include "arch/x86/include/ps2.h"
#include "drivers/include/kbd_scanset1.h"
#include "drivers/include/kbd_scanset2.h"

ATKBD::ATKBD() {}

ATKBD::ATKBD(PIC *pic, PS2 *psc) {
  this->psc = psc;
  this->pic = pic;
  ledStatus = 0x2;
}

void ATKBD::Initialize() {
  uint8_t err;

  idkbd();
  
  setScanSet();
  updateLeds();

  psc->WriteCommand(ENABLE, PORT1);
  err = pic->EnableInterrupt(1);
  if (err < 0) {
    kprint("keyboard error: could not enable keyboard interrupts\n\0");
  }
  psc->EnableInterrupt1();
}

void ATKBD::idkbd() {
  uint8_t err;

  // if you remember from your childhood. the keyboard always had a purple
  // socket to be connected with the purple port in the PC. this is port 1
  psc->WriteCommand(GET_ID, PORT1);
  err = psc->ReadData(&id[0]);
  if (!err) {
    psc->ReadData(&id[1]);
  } else {
    kprint("could not ID keyboard\n\0");
  }
}

void ATKBD::setScanSet() {
  uint8_t scanset;
  uint8_t err;

  psc->WriteCommand(MOD_SCANSET, PORT1); 
  psc->WriteCommand(0, PORT1);
  err = psc->ReadData(&scanset);
  if (err) {
    kprint("could not get keyboard scanset\n\0");
  }

  while (scanset == ACK) {
    err = psc->ReadData(&scanset);
  }

  while (scanset != 0x2) {
    psc->WriteCommand(MOD_SCANSET, PORT1);
    scanset = psc->WriteCommand(2, PORT1);
    err = psc->ReadData(&scanset);
    if (err) {
      kprint("could not get keyboard scanset\n\0");
    }

    while (scanset == ACK) {
      err = psc->ReadData(&scanset);
    }

    if (scanset == 0x1){
      kprint("set keyboard scanset to 1\n\0");
    } else if (scanset == 0x2){
      kprint("set keyboard scanset to 2\n\0");
    } else if (scanset == 0x3){
      kprint("set keyboard scanset to 3\n\0");
    } else {
      kprintf("unknown scanset %x\n\0", scanset);
    }
  }
}

void ATKBD::updateLeds() {
  psc->WriteCommand(SET_LEDS, PORT1);
  psc->WriteCommand(ledStatus, PORT1);
}

void ATKBD::printKeyboardStatus() {
  uint8_t status;
  status = psc->ReadStatus();
  kprintf("read the status of the keyboard %b\n\0", status);
}

void ATKBD::handleData(uint8_t data) {
  uint32_t printData = (uint32_t) data;
  kprintf("recieved data %x\n\0", printData);
  screen.WriteCharacterToScreen(SC1_ToASCII(printData));
}

void ATKBD::Handler() {
  uint8_t data, err;
  err = psc->ReadData(&data);
  if (!err) {
    handleData(data);
  }

  pic->SendEOI(1);
}

void KeyboardHandler(struct interruptFrame *hwregs) { kbdDriver.Handler(); }
