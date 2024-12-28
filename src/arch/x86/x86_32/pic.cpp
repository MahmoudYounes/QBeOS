#include "arch/x86/include/pic.h"
#include "include/common.h"

PIC::PIC() {
#if DISABLE_PIC
  DisablePIC();
  return;
#else

  Initialize();
#endif
}

void PIC::DisablePIC() {
  // intel chips normally come with 2 pics to act as master and slave. this is
  // called legace interrupt delivery mechanism. this is deprecated in favor of
  // the more modern APIC interrupt delivery mechanism. hence, we are disabling
  // the PIC intentionally.
  //
  // The PIC has 4 serial addresses 0x20 0x21 for commands for master and slave
  // and 0xA0 0xA1 for data for master and slave
  kprint("Disabling PIC...\n\0");
  CLAI();
}

void PIC::Initialize() {
  outb(MASTER_CMDPORT, ICW1);
  outb(SLAVE_CMDPORT, ICW1);

  outb(MASTER_DPORT, ICW2M);
  outb(SLAVE_DPORT, ICW2S);

  outb(MASTER_DPORT, ICW3M);
  outb(SLAVE_DPORT, ICW3S);

  outb(MASTER_DPORT, ICW4);
  outb(SLAVE_DPORT, ICW4);

  CLAI();
  masterIMRStatus = 0xff;
  slaveIMRStatus = 0xff;
}

uint8_t PIC::getIMRM() { return masterIMRStatus; }

uint8_t PIC::getIMRS() { return slaveIMRStatus; }

void PIC::STI() {
  outb(MASTER_DPORT, masterIMRStatus);
  outb(SLAVE_DPORT, slaveIMRStatus);
}

// Set All Interrupts
// force sets all the interrupts to enable
void PIC::STAI() {
  outb(MASTER_DPORT, 0);
  outb(SLAVE_DPORT, 0);
}

void PIC::CLI() {
  masterIMRStatus = getIMRM();
  slaveIMRStatus = getIMRS();

  outb(MASTER_DPORT, 0xff);
  outb(SLAVE_DPORT, 0xff);
}

// Clear All Interrupts
// force clears all the interrupts
void PIC::CLAI() {
  outb(MASTER_DPORT, 0xff);
  outb(SLAVE_DPORT, 0xff);
}

void PIC::SendEOI(uint8_t irq) {
  if (irq > 8) {
    outb(SLAVE_CMDPORT, EOI);
  }

  outb(MASTER_CMDPORT, EOI);
}

int8_t PIC::EnableInterrupt(uint8_t irqn) {
  uint8_t r = irqn;

  if (irqn >= 16) {
    return -1;
  }

  if (irqn >= 8) {
    r = irqn % 8;
    r = 1 << r;
    r = ~r;
    slaveIMRStatus &= r;
    outb(SLAVE_DPORT, slaveIMRStatus);
  } else {
    r = 1 << r;
    r = ~r;
    masterIMRStatus &= r;
    outb(MASTER_DPORT, masterIMRStatus);
  }

  return 0;
}

int8_t PIC::DisableInterrupt(uint8_t irqn) {
  uint8_t r = irqn;

  if (irqn >= 16) {
    return -1;
  }

  if (irqn >= 8) {
    r = irqn % 8;
    r = 1 << r;
    slaveIMRStatus |= r;
    outb(SLAVE_DPORT, slaveIMRStatus);
  } else {
    r = 1 << r;
    masterIMRStatus |= r;
    outb(MASTER_DPORT, masterIMRStatus);
  }
  return 0;
}

uint16_t PIC::GetIRR() {
  outb(MASTER_CMDPORT, READ_IRR);
  outb(SLAVE_CMDPORT, READ_IRR);
  return (inb(SLAVE_CMDPORT) << 8) | (inb(MASTER_CMDPORT));
}

uint16_t PIC::GetISR() {
  outb(MASTER_CMDPORT, READ_ISR);
  outb(SLAVE_CMDPORT, READ_ISR);
  return (inb(SLAVE_CMDPORT) << 8) | (inb(MASTER_CMDPORT));
}

uint16_t PIC::GetIMR(){
  return (getIMRS() << 8) | getIMRM();
}
