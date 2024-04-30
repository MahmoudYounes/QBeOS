#include "arch/x86/include/tss.h"

TSSManager::TSSManager() {
  uint8_t *kernelStack = (uint8_t *)vmm.Allocate(2 * PAGE_SIZE_BYTES);
  currTask.ssp0 = GDT_KERNEL_DATA_DESCRIPTOR_SEL;
  currTask.sp0 = (generalreg_size_t)kernelStack +
                 (2 * PAGE_SIZE_BYTES); // this is the top of the stack
  GDTEntry entry = gdt.ConstructTSSKernEntry((uintptr_t)&currTask, sizeof(tss));
  currTaskSelector = gdt.AddGDTEntry(&entry);
  gdt.RefreshGDT();
  __asm__ __volatile__("ltr %0" : : "r"(currTaskSelector));
}

uint32_t TSSManager::GetCurrentTask() { return 0; }

TSSManager tssManager;
