#include "arch/x86/include/apic.h"

APIC::APIC() {
  kprint("Initializing APIC\n\0");
  enableAPIC();
  readRegs();
}

void APIC::enableAPIC() {
  uint32_t eax, edx;
  char buf[512];

  if (cpu.IsCPUFeatureSupported(CPU_FEAT_APIC)) {
    kprint("CPU Supports APIC\n\0");
  } else {
    panic("APIC is not supported.. your CPU arch is not supported by QBeOS "
          "yet\n\0");
  }

  if (cpu.IsCPUFeatureSupported(CPU_FEAT_X2APIC)) {
    kprint("CPU Supports x2APIC\n\0");
    supportedX2APIC = true;
  }

  // reads the MSR APIC_ADDR_BASE
  RDMSR(0x1b, eax, edx);

  // bit 8 shows if this is the bootstrap processor BSP
  // set bit 10 to enable x2APIC
  // set bit 11 to enable global APIC
  // bits 12:31 on 32bit arch or 12:63 on 64bit arch constitute APIC base
  // bits 12:35 on 32bit arch with support for 36 bit addressing space
  initialRegistersAddress = eax & 0xfffff000;
  isBSP = eax & 0x100;
  kprintf(buf, "Found APIC. base addr %x has x2apic %d is BSP %d\n\0",
          initialRegistersAddress, supportedX2APIC, isBSP);

  // must configure APIC here. IMP!!: no memory writes or apic enable before you
  // virtually map the memory;
  // APIC registers are memory mapped and the cache control for the APIC memory
  // region should be strongly uncachable. 12.3
  // APIC overview pg3389 -- figure 11.4 -- 11.3
  // APIC register specs pg3390 -- table 11-1 -- 11.3
  //
  // 11.4.7 APIC states
  //
  // APIC important information to parse:
  // - APICID from mmaped reg 0 24:end
  // - APICVersion from APIC Version register
  // - max LVT enteries from APIC Version register (LVT Entries - 1)
  //
  // APIC LVT Registers:
  // - LVT CMCI: when correctable machine check errors happen.
  // - LVT Timer: programmable interrupt timer
  // - LVT Thermal monitor
  // - LVT Performance counter
  // - LVT LINT0
  // - LVT LINT1
  // - LVT Error - internal APIC error
  //
  // information for the configuration of the APIC registers
  // - vector number 0..7
  // - deliver mode 8,9,10: 0 deliver interrupt in vector, 2 (sys man
  // interrupt), 4 NMI, 5 INIT processor, 6 res,
  //   7 ExtINT (as if coming from PIC)
  // - delivery status readonly
  // - pin polarity: 0 active high, 1 active low
  // - Remote IRR (readonly): only valid for fixed edge triggered. set when
  // APIC.
  //   processes interrupt reset on EOI from the processor.
  // - Trigger mode: only valid for fixed. 0 edge sensitive, 1 level sensitive.
  //   others: edge sensitive. LINT1 is always edge sensitive must be set
  //   manually.
  // - Mask: 0 enable interrupt. 1 disable interrupt.
  // - Timer mode: 0 one-shot, 1 periodic, 2 TSC deadline.
  //
  // APIC Error handling interrupt
  // When internal error in lapic occurs the ESR contains the error.
  // - b0: send checksum error. when apic sends message on bus and checksum is
  // invalid.
  // - b1: recieve checksum error. when apic recieves message and
  // checksum is invalid.
  // - b2: send accept error. when apic sends message not
  // accepted by any other apic.
  // - b3: recieve accept error. when apic recieves a
  // message not accepted by any other apic including itself.
  // - b4: redirectable IPI can't be sent.
  // - b5: send illegal vectori
  // - b6: recieved illegal vector
  // - b7: illegal register address
  //
  // APIC Timer:
  // programmed with 4 registers. divide configuration register, initial count,
  // current count, and LVT timer
  //
  // divide configuration register [0,1,3]: divide by number
  // - 0: 2
  // - 1: 4
  // - 2: 8
  // - 3: 16
  // - 4: 32
  // - 5: 64
  // - 6: 128
  // - 7: 1
  // initial count register: start timer by writing to this register
  // current count register: read only. current value of timer.
  // LVT register: programs the interrupt to be sent.
  // for TSC need to check for the feature in processor. if enabled TSC is
  // supported if not TSC is not supported.
  // TSC controlled via IA32_TSC_DEADLINE MSR (6e0h) writes to initial time
  // counter register are ignored. writes to MSR rearms the timer.
  //
  //

  if (supportedX2APIC) {
    eax |= 0x400; // set bit 10;
  }

  eax |= 0x800; // set bit 11;
  WRMSR(0x1b, eax, edx);
  kprint("Enabled APIC...\n\0");
}

APICRegs *APIC::readRegs() { return &regs; }

void APIC::writeRegs() {}

APIC apic; // should be removed this must be defined per CPU Core;
