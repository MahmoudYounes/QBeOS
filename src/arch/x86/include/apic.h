#ifndef APIC_H
#define APIC_H

#include "arch/x86/include/cpuinfo.h"
#include "arch/x86/include/processor.h"
#include "include/common.h"
#include "include/logger.h"

extern CPUInfo cpu;

struct APICRegs {
  uint32_t id;         // RW
  uint32_t version;    // R
  uint32_t tpr;        // task priority register RW
  uint32_t apr;        // arbitration priority register R
  uint32_t ppr;        // processor priority register R
  uint32_t eoi;        // end of interrupt W
  uint32_t rrd;        // remote read register R
  uint32_t ldr;        // logical destination register RW
  uint32_t dfr;        // destination format register RW
  uint32_t spir;       // spurious interrupt vector register RW
  uint32_t isr0;       // interrupt service register R
  uint32_t isr1;       // interrupt service register R
  uint32_t isr2;       // interrupt service register R
  uint32_t isr3;       // interrupt service register R
  uint32_t isr4;       // interrupt service register R
  uint32_t isr5;       // interrupt service register R
  uint32_t isr6;       // interrupt service register R
  uint32_t isr7;       // interrupt service register R
  uint32_t tmr0;       // trigger mode register R
  uint32_t tmr1;       // trigger mode register R
  uint32_t tmr2;       // trigger mode register R
  uint32_t tmr3;       // trigger mode register R
  uint32_t tmr4;       // trigger mode register R
  uint32_t tmr5;       // trigger mode register R
  uint32_t tmr6;       // trigger mode register R
  uint32_t tmr7;       // trigger mode register R
  uint32_t irr0;       // interrupt request register R
  uint32_t irr1;       // interrupt request register R
  uint32_t irr2;       // interrupt request register R
  uint32_t irr3;       // interrupt request register R
  uint32_t irr4;       // interrupt request register R
  uint32_t irr5;       // interrupt request register R
  uint32_t irr6;       // interrupt request register R
  uint32_t irr7;       // interrupt request register R
  uint32_t esr;        // error status register RW
  uint32_t lvtcmci;    // local vector table corrected machine check interrupt
                       // register RW
  uint32_t icr_lo;     // interrupt command register low RW
  uint32_t icr_hi;     // interrupt command register high RW
  uint32_t lvttimer;   // local vector table timer register RW
  uint32_t lvtthremal; // local vector table thermal register RW
  uint32_t lvtpmc;     // local vector table performance counter RW
  uint32_t lvtlint0;   // local vector table LINT[0] pin RW
  uint32_t lvtlint1;   // local vector table LINT[1] pin RW
  uint32_t lvterr;     // local vector table error register RW
  uint32_t icr;        // initial count register RW
  uint32_t ccr;        // current count register R
  uint32_t dcr;        // divide configuration register RW
};

// Used to handle all APIC functionalities.
// Every processor should have one object of this class.
class APIC {
private:
  // Address Space for communicating with APIC.
  // Contains the APIC Table
  bool isMapped = false;
  uintptr_t initialRegistersAddress;
  uintptr_t remappedRegistersAddress;

  // APIC Info
  APICRegs regs;
  bool supportedAPIC;
  bool supportedX2APIC;
  bool isBSP;

  // init utils
  void enableAPIC();

  // APIC RW
  APICRegs *readRegs();
  void writeRegs();

public:
  APIC();
};

#endif /* APIC_H */
