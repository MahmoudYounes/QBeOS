#ifndef TSS_H
#define TSS_H

#include "include/common.h"
#include "include/logger.h"
#include "arch/x86/include/processor.h"
#include "arch/include/vmm.h"
#include "arch/x86/include/gdt.h"

extern VirtualMemory vmm;
extern GDT gdt;

struct tss {
  uint32_t prevTask;
  generalreg_size_t sp0;
  segmentreg_size_t ssp0;
  uint16_t _res0;
  generalreg_size_t sp1;
  generalreg_size_t ssp1;
  uint16_t _res1;
  generalreg_size_t esp2;
  generalreg_size_t ssp2;
  uint16_t _res2;
  controlreg_size_t cr3;
  generalreg_size_t ip;
  generalreg_size_t eflags;
  generalreg_size_t eax;
  generalreg_size_t ecx;
  generalreg_size_t edx;
  generalreg_size_t ebx;
  generalreg_size_t esp;
  generalreg_size_t ebp;
  generalreg_size_t esi;
  generalreg_size_t edi;
  segmentreg_size_t es;
  uint16_t _res3;
  segmentreg_size_t cs;
  uint16_t _res4;
  segmentreg_size_t ss;
  uint16_t _res5;
  segmentreg_size_t ds;
  uint16_t _res6;
  segmentreg_size_t fs;
  uint16_t _res7;
  segmentreg_size_t gs;
  uint16_t _res8;
  segmentreg_size_t ldt;
  uint16_t _res9;
  generalreg_size_t iomap;
};

// this controls the TSS structure.
// should be initialized after vmm
class TSSManager {
public:
    TSSManager();
};

#endif /* TSS_H */
