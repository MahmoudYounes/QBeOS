#include "drivers/include/atkbd.h"
#include "arch/x86/include/ps2.h"

ATKBD::ATKBD(){}

ATKBD::ATKBD(PIC *pic, PS2 *psc){ 
  this->psc = psc;
  this->pic = pic;
}

void ATKBD::Initialize(){
  int8_t id[2], err;

  // if you remember from your childhood. the keyboard always had a purple
  // socket to be connected with the purple port in the PC
  //psc->WriteCommand(0xf2, PORT1);
  //id[0] = psc->ReadData();
  //id[1] = psc->ReadData();
  
  // we probably should have read a zero here so we set it back
  //if (id[0] == ERRNO_RDATA){
  //  id[0] = 0;
  //}
  //if(id[1] == ERRNO_RDATA){
  //  id[1] = 0;
  //}
  //for (int i=0; i < 1000000;i++){}
  //psc->FlushOutput();
  //psc->FlushOutput();
  err = pic->EnableInterrupt(1);
  if (err < 0){
    kprint("keyboard error: could not enable keyboard interrupts\n\0");   
  }
  //for (int i=0; i < 1000000;i++){err = i;}

  //psc->EnableInterrupt1();  
  //for (int i=0; i < 1000000;i++){}

  //psc->WriteCommand(ENABLE, PORT1);
}

void ATKBD::printKeyboardStatus(){
  uint8_t status;
  status = psc->ReadStatus();
  kprintf("read the status of the keyboard %b\n\0", status);

}

void ATKBD::Handler(){
  int8_t data;
  uint8_t max_trials = 0xfe, curr_trials=0;
  uint16_t isr, irr;
  printKeyboardStatus();
  data = psc->ReadData();
  //data = psc->ReadData();
  //data = psc->ReadData();


  kprintf("read data %x\n\0", (uint32_t)data);
  psc->FlushOutput();
  
  isr = pic->GetISR();
  kprintf("read isr %b\n\0", (uint32_t) isr);

  irr = pic->GetIRR();
  kprintf("read irr %b\n\0", (uint32_t) irr);
  pic->SendEOI(1);

  isr = pic->GetISR();
  kprintf("read isr %b\n\0", (uint32_t) isr);

  irr = pic->GetIRR();
  kprintf("read irr %b\n\0", (uint32_t) irr);
}


void KeyboardHandler(struct interruptFrame *hwregs){
  kprint("recieved keyboard interrupt\n\0");
  kbdDriver.Handler(); 
}

