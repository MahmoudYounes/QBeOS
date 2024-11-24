#include "drivers/include/atkbd.h"
#include "arch/x86/include/ps2.h"

ATKBD::ATKBD(){}

ATKBD::ATKBD(PIC *pic, PS2 *psc){ 
  this->psc = psc;
  this->pic = pic;
}

void ATKBD::Initialize(){
  uint8_t id[2];
  uint8_t err;

  // if you remember from your childhood. the keyboard always had a purple
  // socket to be connected with the purple port in the PC
  psc->WriteCommand(0xf2, PORT1);
  err = psc->ReadData(&id[0]);
  if (!err){
    psc->ReadData(&id[1]);
  } else {
    kprint("could not ID keyboard\n\0");
  }
   
  err = pic->EnableInterrupt(1);
  if (err < 0){
    kprint("keyboard error: could not enable keyboard interrupts\n\0");   
  }
  psc->WriteCommand(ENABLE, PORT1);
  enableLeds();
}

void ATKBD::enableLeds(){
  psc->WriteCommand(SET_LEDS, PORT1);
  psc->WriteCommand(0x2, PORT1);
}

void ATKBD::printKeyboardStatus(){
  uint8_t status;
  status = psc->ReadStatus();
  kprintf("read the status of the keyboard %b\n\0", status);
}

void ATKBD::handleData(uint8_t data){
  kprintf("recieved data %x\n\0", data);

}

void ATKBD::Handler(){
  uint8_t data, err;
  err = psc->ReadData(&data);
  if (!err){
    handleData(data);    
  }
 
  pic->SendEOI(1);
}


void KeyboardHandler(struct interruptFrame *hwregs){
  kbdDriver.Handler(); 
}

