#include "arch/x86/include/tss.h"

TSSManager::TSSManager() {
  uint8_t *kernelStack = (uint8_t *)vmm.Allocate(2 * PAGE_SIZE_BYTES);
  currTask.ss0 = GDT_KERNEL_DATA_DESCRIPTOR_SEL;

  uint8_t *userStack = (uint8_t *)vmm.Allocate(2 * PAGE_SIZE_BYTES);
  currTask.ss2 = GDT_USER_CODE_DESCRIPTOR_SEL;
  currTask.sp2 = (generalreg_size_t)userStack + (2 * PAGE_SIZE_BYTES);

  GDTEntry entry = gdt.ConstructTSSKernEntry((uintptr_t)&currTask, sizeof(tss));
  currTaskSelector = gdt.AddGDTEntry(&entry);
  gdt.RefreshGDT();
  __asm__ __volatile__("ltr %0" : : "r"(currTaskSelector));
}

tss *TSSManager::GetCurrentTask() { return &currTask; }

uint32_t TSSManager::GetCurrentTaskSelector() { return currTaskSelector; }

TSSManager tssManager;
