#include "include/apic.h"

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
    supportedAPIC = true;
  }

  if (cpu.IsCPUFeatureSupported(CPU_FEAT_X2APIC)) {
    kprint("CPU Supports x2APIC\n\0");
    supportedX2APIC = true;
  }

  if (!supportedAPIC) {
    panic("APIC is not supported.. your CPU arch is not supported by QBeOS "
          "yet\n\0");
  }

  // reads the MSR APIC_ADDR_BASE
  RDMSR(0x1b, eax, edx);

  // bit 8 shows if this is the bootstrap processor BSP
  // set bit 10 to enable x2APIC
  // set bit 11 to enable global APIC
  // bits 12:31 on 32bit arch or 12:63 on 64bit arch constitute APIC base
  // address
  initialRegistersAddress = eax & 0xfffff000;
  isBSP = eax & 0x100;
  kprintf(buf, "Found APIC. base addr %x has x2apic %d is BSP %d\n\0",
          initialRegistersAddress, supportedX2APIC, isBSP);

  // must configure APIC here. IMP!!: no memory writes or apic enable before you
  // virtually map the memory;

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
