#include "arch/x86/include/apic.h"

APIC::APIC() {
  kprint("Initializing APIC\n\0");
  validateAPIC();
  initializeAPIC();
  enableAPIC();
}

void APIC::validateAPIC() {

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
}

void APIC::initializeAPIC() {
  uint32_t eax, edx;
  char *buf = new char[512];

  // reads the MSR APIC_ADDR_BASE
  RDMSR(0x1b, eax, edx);

  initialRegistersAddress = eax & 0xfffff000;
  isBSP = eax & 0x100;

  vmm.MMap(initialRegistersAddress, initialRegistersAddress);
  kprintf(buf, "APIC memory mapped to %x\n\0", initialRegistersAddress);

  uint32_t id = readRegister(ID_REG);
  uint32_t versionReg = readRegister(VER_REG);
  uint32_t apicVersion = versionReg & 0xff;
  versionReg >>= 16;
  uint32_t maxLVT = (versionReg & 0xff) + 1;
  versionReg >>= 8;
  bool EOIBroadcastSuppressed = versionReg & 0x1;

  kprintf(buf, "Initial APIC ID: %d\n\0", id);
  kprintf(buf, "Initial APIC Version: %d\n\0", apicVersion);
  kprintf(buf, "Supports %d LVTs\n\0", maxLVT);
  kprintf(buf, "Supports EOI Broadcast %d\n\0", EOIBroadcastSuppressed);

  configureCMCI();
  configureTimer();
  configureThermal();
  configurePMC();
  configureLINT0();
  configureLINT1();
  configureErr();
  configureSpurious();
}

void APIC::configureSpurious() {
  uint32_t configLoad = 0x1ff; // int 37
  writeRegister(SIV_REG, configLoad);
}

void APIC::configureCMCI() {
  uint32_t configLoad = 0x20; // int 32
  writeRegister(LVT_CMCI_REG, configLoad);
}

void APIC::configureTimer() {
  uint32_t dcrLoad = 0x2; // divide by 8 for some reason.
  writeRegister(DIV_CFG_REG, dcrLoad);

  uint32_t configLoad = 0x21; // int 33
  configLoad &= 0 << 8;       // fixed delivery
  configLoad &= 1 << 17;      // timer mode: periodic
  writeRegister(LVT_TMR_REG, configLoad);
}

void APIC::configureThermal() {
  // masked by default
}

void APIC::configurePMC() {
  uint32_t configLoad = 0x22; // it 34
  writeRegister(LVT_PMC_REG, configLoad);
}

void APIC::configureLINT0() {
  uint32_t configLoad = 0x23; // int 35
  writeRegister(LVT_LINT0_REG, configLoad);
}

void APIC::configureLINT1() {
  uint32_t configLoad = 0x24; // int 36
  writeRegister(LVT_LINT1_REG, configLoad);
}

void APIC::configureErr() {
  // masked by default
}

void APIC::enableAPIC() {
  uint32_t eax, edx;

  // reads the MSR APIC_ADDR_BASE
  RDMSR(0x1b, eax, edx);

  if (supportedX2APIC) {
    eax |= 0x400; // set bit 10;
  }

  eax |= 0x800; // set bit 11;
  WRMSR(0x1b, eax, edx);
  kprint("Enabled APIC...\n\0");
}

int32_t APIC::readRegister(uint32_t reg) {
  if (isReserved(reg)) {
    return -1;
  }

  return *(uint32_t *)(initialRegistersAddress + reg);
}

void APIC::writeRegister(uint32_t reg, uint32_t content) {
  if (isReserved(reg))
    return;
  if (isROnly(reg))
    return;

  *(uint32_t *)(initialRegistersAddress + reg) = content;
}

// TODO: replace with macros
bool APIC::isReserved(uint32_t reg) { return reg != ID_REG && reg != VER_REG; }

bool APIC::isROnly(uint32_t reg) {
  return reg == ISR0_REG || reg == ISR1_REG || reg == ISR2_REG ||
         reg == ISR3_REG || reg == ISR4_REG || reg == ISR5_REG ||
         reg == ISR6_REG || reg == ISR7_REG;
}

void APIC::StartTimer(uint32_t val) { writeRegister(INIT_COUNT_REG, val); }

uint32_t APIC::ReadTimer() { return readRegister(CURR_COUNT_REG); }

APIC apic; // should be removed this must be defined per CPU Core;
