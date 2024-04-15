#include "arch/x86/include/tss.h"

TSSManager::TSSManager() {
  uint8_t *kernelStack = (uint8_t *)vmm.Allocate(2 * PAGE_SIZE_BYTES);
  tss *initTSS = new tss();
  initTSS->ssp0 = GDT_KERNEL_DATA_DESCRIPTOR_SEL;
  initTSS->sp0 = (generalreg_size_t)kernelStack +
                 (2 * PAGE_SIZE_BYTES); // this is the top of the stack
  GDTEntry entry = gdt.ConstructTSSKernEntry((uintptr_t)initTSS, sizeof(tss));
  gdt.AddGDTEntry(&entry);
  gdt.RefreshGDT();
}

TSSManager tssManager;
