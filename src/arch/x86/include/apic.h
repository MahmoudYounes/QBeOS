#ifndef APIC_H
#define APIC_H

#include "include/configs.h"
#include "arch/include/vmm.h"
#include "arch/x86/include/cpuinfo.h"
#include "arch/x86/include/processor.h"
#include "include/common.h"
#include "include/logger.h"

extern CPUInfo cpu;

// 0x3f0 is the last offset for apic register which is 4 bytes as well
#define SIZEOF_APIC_TABLE 0x3f4
#define ID_REG 0x20
#define VER_REG 0x30
#define TASK_PRIORITY_REG 0x80
#define ARBITR_PRIORITY_REG 0x90
#define CPU_PRIORITY_REG 0xa0
#define EOI_REG 0xb0
#define REMOTE_READ_REG 0xc0
#define LOGICAL_DESTINATION_REG 0xd0
#define DESTINATION_FORMAT_REG 0xe0
#define SIV_REG 0xf0 // spurious interrupt vector register
#define ISR0_REG 0x100
#define ISR1_REG 0x110
#define ISR2_REG 0x120
#define ISR3_REG 0x130
#define ISR4_REG 0x140
#define ISR5_REG 0x150
#define ISR6_REG 0x160
#define ISR7_REG 0x170
#define TMR0_REG 0x180
#define TMR1_REG 0x190
#define TMR2_REG 0x1a0
#define TMR3_REG 0x1b0
#define TMR4_REG 0x1c0
#define TMR5_REG 0x1d0
#define TMR6_REG 0x1e0
#define TMR7_REG 0x1f0
#define IRR0_REG 0x200
#define IRR1_REG 0x210
#define IRR2_REG 0x220
#define IRR3_REG 0x230
#define IRR4_REG 0x240
#define IRR5_REG 0x250
#define IRR6_REG 0x260
#define IRR7_REG 0x270
#define ERR_REG 0x280
#define LVT_CMCI_REG 0x2f0
#define LVT_ICR0_REG 0x300
#define LVT_ICR1_REG 0x310
#define LVT_TMR_REG 0x320
#define LVT_THRML_REG 0x330
#define LVT_PMC_REG 0x340
#define LVT_LINT0_REG 0x350
#define LVT_LINT1_REG 0x360
#define LVT_LINTERR_REG 0x370
#define INIT_COUNT_REG 0x380
#define CURR_COUNT_REG 0x390
#define DIV_CFG_REG 0x3e0

enum DIV_CFG {
  DIV2 = 0x0,
  DIV4 = 0x1,
  DIV8 = 0x2,
  DIV16 = 0x3,
  DIV32 = 0x8,
  DIV64 = 0x9,
  DIV128 = 0xa,
  DIV1 = 0xb
};

enum TIMER_MODE {
  ONESHOT = 0x0,
  PERIODIC = 0x1,
  TSCD = 0x2,
};

class APIC {
private:
  bool isInitialized;
  bool isMapped = false;
  uintptr_t initialRegistersAddress;
  bool supportedAPIC;
  bool supportedX2APIC;
  bool isBSP;
  bool isReserved(uint32_t reg);
  bool isROnly(uint32_t reg);
  void validateAPIC();
  void initializeAPIC();
  void enableAPIC();
  void configureCMCI();
  void configureTimer();
  void configureThermal();
  void configurePMC();
  void configureLINT0();
  void configureLINT1();
  void configureErr();
  void configureSpurious();

  int32_t readRegister(uint32_t reg);
  void writeRegister(uint32_t reg, uint32_t content);

public:
  APIC();
  void StartTimer(uint32_t val);
  uint32_t ReadTimer();
  void DebugPrintAPICState();
  void WriteEOI();
  void StartTimer(DIV_CFG divCfg, TIMER_MODE mode, uint32_t initVal);
};

#endif /* APIC_H */
