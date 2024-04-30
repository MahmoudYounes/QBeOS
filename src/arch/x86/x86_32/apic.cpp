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
  isBSP = eax & (1 << 8);

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

  isInitialized = true;
}

void APIC::configureSpurious() {
  uint32_t configLoad = 0xff; // 254
  configLoad |= 0x100;
  writeRegister(SIV_REG, configLoad);
}

void APIC::configureCMCI() {
  uint32_t configLoad = 0x20; // int 32
  writeRegister(LVT_CMCI_REG, configLoad);
}

void APIC::configureTimer() {
  uint32_t configLoad = 0x21; // int 33
  writeRegister(LVT_TMR_REG, configLoad);
}

void APIC::StartTimer(DIV_CFG divCfg, TIMER_MODE mode, uint32_t initVal) {
  if (!isInitialized) {
    panic("tried to use the APIC without initialization\n\0");
  }
  writeRegister(DIV_CFG_REG, divCfg);

  uint32_t timerCfg = readRegister(LVT_TMR_REG);
  timerCfg |= mode << 17; // timer mode: periodic
  writeRegister(LVT_TMR_REG, timerCfg);

  StartTimer(initVal);
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
  uint32_t configLoad = 0x25; // int 37
  writeRegister(LVT_LINTERR_REG, configLoad);
}

void APIC::enableAPIC() {
  uint32_t eax, edx;

  // reads the MSR APIC_ADDR_BASE
  RDMSR(0x1b, eax, edx);

  // if (supportedX2APIC) {
  //   eax |= 0x400; // set bit 10;
  // }

  eax |= 1 << 11;
  WRMSR(0x1b, eax, edx);
  configureSpurious();
  kprint("Enabled APIC...\n\0");
}

int32_t APIC::readRegister(uint32_t reg) {
  if (isReserved(reg)) {
    return -1;
  }

  char *buf = new char[512];
  uint32_t *regMemAddr = (uint32_t *)(initialRegistersAddress + reg);
  kprintf(buf, "reading contents of memory address %x\n\0", regMemAddr);
  return *(uint32_t *)(initialRegistersAddress + reg);
}

void APIC::writeRegister(uint32_t reg, uint32_t content) {
  if (isReserved(reg))
    panic("tried to write a reserved register in APIC config\n\0");
  if (isROnly(reg))
    panic("tried to write a read only register in APIC config\n\0");

  *(uint32_t *)(initialRegistersAddress + reg) = content;
}

// TODO: replace with macros
bool APIC::isReserved(uint32_t reg) {
  return reg != ID_REG && reg != VER_REG && reg != TASK_PRIORITY_REG &&
         reg != ARBITR_PRIORITY_REG && reg != CPU_PRIORITY_REG &&
         reg != EOI_REG && reg != REMOTE_READ_REG &&
         reg != LOGICAL_DESTINATION_REG && reg != DESTINATION_FORMAT_REG &&
         reg != SIV_REG && reg != ISR0_REG && reg != ISR1_REG &&
         reg != ISR2_REG && reg != ISR3_REG && reg != ISR4_REG &&
         reg != ISR5_REG && reg != ISR6_REG && reg != ISR7_REG &&
         reg != TMR0_REG && reg != TMR1_REG && reg != TMR2_REG &&
         reg != TMR3_REG && reg != TMR4_REG && reg != TMR5_REG &&
         reg != TMR6_REG && reg != TMR7_REG && reg != IRR0_REG &&
         reg != IRR1_REG && reg != IRR2_REG && reg != IRR3_REG &&
         reg != IRR4_REG && reg != IRR5_REG && reg != IRR6_REG &&
         reg != IRR7_REG && reg != ERR_REG && reg != LVT_CMCI_REG &&
         reg != LVT_ICR0_REG && reg != LVT_ICR1_REG && reg != LVT_TMR_REG &&
         reg != LVT_THRML_REG && reg != LVT_PMC_REG && reg != LVT_LINT0_REG &&
         reg != LVT_LINT1_REG && reg != LVT_LINTERR_REG &&
         reg != INIT_COUNT_REG && reg != CURR_COUNT_REG && reg != DIV_CFG_REG;
}
bool APIC::isROnly(uint32_t reg) {
  return reg == VER_REG || reg == ARBITR_PRIORITY_REG ||
         reg == CPU_PRIORITY_REG || reg == REMOTE_READ_REG || reg == ISR0_REG ||
         reg == ISR1_REG || reg == ISR2_REG || reg == ISR3_REG ||
         reg == ISR4_REG || reg == ISR5_REG || reg == ISR6_REG ||
         reg == ISR7_REG || reg == TMR0_REG || reg == TMR1_REG ||
         reg == TMR2_REG || reg == TMR3_REG || reg == TMR4_REG ||
         reg == TMR5_REG || reg == TMR6_REG || reg == TMR7_REG ||
         reg == IRR0_REG || reg == IRR1_REG || reg == IRR2_REG ||
         reg == IRR3_REG || reg == IRR4_REG || reg == IRR5_REG ||
         reg == IRR6_REG || reg == IRR7_REG || reg == CURR_COUNT_REG;
}

void APIC::StartTimer(uint32_t val) {
  writeRegister(TASK_PRIORITY_REG, 0);
  writeRegister(INIT_COUNT_REG, val);
}

uint32_t APIC::ReadTimer() { return readRegister(CURR_COUNT_REG); }

void APIC::DebugPrintAPICState() {
  char *buf = new char[512];
  uint32_t val = readRegister(ID_REG);
  kprintf(buf, "ID: %d %x %b\n\0", val, val, val);

  val = readRegister(VER_REG);
  kprintf(buf, "Version: %d %x %b\n\0", val, val, val);

  val = readRegister(TASK_PRIORITY_REG);
  kprintf(buf, "Task Priority: %d %x %b\n\0", val, val, val);

  val = readRegister(ARBITR_PRIORITY_REG);
  kprintf(buf, "Arbitration Priority: %d %x %b\n\0", val, val, val);

  val = readRegister(CPU_PRIORITY_REG);
  kprintf(buf, "CPU Priority: %d %x %b\n\0", val, val, val);

  val = readRegister(EOI_REG);
  kprintf(buf, "EOI: %d %x %b\n\0", val, val, val);

  val = readRegister(SIV_REG);
  kprintf(buf, "Spurious Interrupt LVT Register: %d %x %b\n\0", val, val, val);

  val = readRegister(ERR_REG);
  kprintf(buf, "Error Register: %d %x %b\n\0", val, val, val);

  val = readRegister(LVT_CMCI_REG);
  kprintf(buf, "CMCI LVT: %d %x %b\n\0", val, val, val);

  val = readRegister(LVT_TMR_REG);
  kprintf(buf, "Timer LVT: %d %x %b\n\0", val, val, val);

  val = readRegister(LVT_PMC_REG);
  kprintf(buf, "PMC LVT: %d %x %b\n\0", val, val, val);

  val = readRegister(LVT_LINT0_REG);
  kprintf(buf, "LINT0 LVT: %d %x %b\n\0", val, val, val);

  val = readRegister(LVT_LINT1_REG);
  kprintf(buf, "LINT1 LVT: %d %x %b\n\0", val, val, val);

  val = readRegister(LVT_LINTERR_REG);
  kprintf(buf, "Err LVT Register: %d %x %b\n\0", val, val, val);

  val = readRegister(INIT_COUNT_REG);
  kprintf(buf, "INIT Count: %d %x %b\n\0", val, val, val);

  val = readRegister(CURR_COUNT_REG);
  kprintf(buf, "Current Count: %d %x %b\n\0", val, val, val);

  val = readRegister(DIV_CFG_REG);
  kprintf(buf, "Divide Configuration: %d %x %b\n\0", val, val, val);
}

void APIC::WriteEOI() {
  uint32_t val = 0xe01;
  writeRegister(EOI_REG, val);
}

APIC apic; // should be removed this must be defined per CPU Core;
