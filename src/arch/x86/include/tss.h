#ifndef TSS_H
#define TSS_H

#include "arch/include/vmm.h"
#include "arch/x86/include/gdt.h"
#include "arch/x86/include/processor.h"
#include "include/common.h"
#include "include/logger.h"

extern GDT gdt;

struct tss {
  generalreg_size_t sp0;
  segmentreg_size_t ss0;
  generalreg_size_t sp2;
  generalreg_size_t ss2;
  controlreg_size_t cr3;
  generalreg_size_t ip;
  generalreg_size_t eflags;
  generalreg_size_t ax;
  generalreg_size_t cx;
  generalreg_size_t dx;
  generalreg_size_t bx;
  generalreg_size_t sp;
  generalreg_size_t bp;
  generalreg_size_t si;
  generalreg_size_t di;
  segmentreg_size_t es;
  segmentreg_size_t cs;
  segmentreg_size_t ss;
  segmentreg_size_t ds;
  segmentreg_size_t fs;
  segmentreg_size_t gs;
};

// this controls the TSS structure.
// should be initialized after vmm
class TSSManager {
private:
  uint32_t currTaskSelector;
  tss currTask;

public:
  TSSManager();
  tss *GetCurrentTask();
  uint32_t GetCurrentTaskSelector();
};

#endif /* TSS_H */
