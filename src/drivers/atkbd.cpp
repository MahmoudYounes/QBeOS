#include "drivers/include/atkbd.h"

ATKBD::ATKBD(PS2 *psc){ 
  this->psc = psc;
}

void ATKBD::Initialize(){
  uint8_t id[2], err;

  // if you remember from your childhood. the keyboard always had a purple
  // socket to be connected with the purple port in the PC
  psc->WriteCommand(0xf2, PORT1);
  id[0] = psc->ReadData();
  id[1] = psc->ReadData();
  
  psc->FlushOutput();
  psc->FlushOutput();
  psc->FlushOutput();

  psc->WriteCommand(ENABLE, PORT1);
  psc->EnableInterrupt1();
  err = pic.EnableInterrupt(1);
  if (err < 0){
    
  }
}

void ATKBD::Handler(){

}

