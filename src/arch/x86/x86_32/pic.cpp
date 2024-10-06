#include "arch/x86/include/pic.h"
#include "include/common.h"

PIC::PIC() {
  if (DISABLE_PIC){
    DisablePIC();
    return;
  }

  initialize();
}

void PIC::DisablePIC(){
  // intel chips normally come with 2 pics to act as master and slave. this is
  // called legace interrupt delivery mechanism. this is deprecated in favor of
  // the more modern APIC interrupt delivery mechanism. hence, we are disabling
  // the PIC intentionally.
  //
  // The PIC has 4 serial addresses 0x20 0x21 for commands for master and slave
  // and 0xA0 0xA1 for data for master and slave
  kprint("Disabling PIC...\n\0");
  CLI();
}

void PIC::initialize(){
  outb(MASTER_CMDPORT, ICW1);
  outb(SLAVE_CMDPORT, ICW1);

  outb(MASTER_DPORT, ICW2M);
  outb(SLAVE_DPORT, ICW2S);

  outb(MASTER_DPORT, ICW3M);
  outb(SLAVE_DPORT, ICW3S);

  outb(MASTER_DPORT, ICW4);
  outb(SLAVE_DPORT, ICW4);

  outb(MASTER_DPORT, 0);
  outb(SLAVE_DPORT, 0);
}

uint8_t PIC::getIMRM(){
  return inb(MASTER_DPORT);
}

uint8_t PIC::getIMRS(){
  return inb(SLAVE_DPORT);
}

void PIC::STI(){
  uint8_t mimr, simr;
  
  mimr = 0; // enable all interrupts on the master chip
  outb(MASTER_DPORT, mimr);

  simr = 0;
  outb(SLAVE_DPORT, simr);
}

void PIC::CLI(){
  uint8_t mimr, simr;
  
  mimr = 0xff; // enable all interrupts on the master chip
  outb(MASTER_DPORT, mimr);

  simr = 0xff;
  outb(SLAVE_DPORT, simr);
}

void PIC::SendEOI(uint8_t irq){
  char buf[256];
  if (irq > 8){
    kprintf(buf, "sending EOI for slave for irq %d\n\0", irq);
    outb(SLAVE_CMDPORT, EOI);
  }

  kprintf(buf, "sending EOI for irq %d\n\0", irq);
  outb(MASTER_CMDPORT, EOI);  
}

