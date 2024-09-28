#ifndef TSS_H
#define TSS_H

#include "arch/include/vmm.h"
#include "arch/x86/include/gdt.h"
#include "arch/x86/include/processor.h"
#include "include/common.h"
#include "include/logger.h"
#include "include/task.h"

extern GDT gdt;

class Task : Kernel::Task {
public:
  Task();
  static Kernel::Task *CreateTask();
  void LoadTask() override;
};

#endif /* TSS_H */
