#include "kstdlib/include/global_operators.h"

void *operator new(size_t size) { 
#if DEBUG
  char buf[256];
  kprintf("allocating %d bytes\n\0", size);
#endif
  return vmm.Allocate(size); 
}

void *operator new[](size_t size) { 
#if DEBUG
  char buf[256];
  kprintf("allocating %d bytes\n\0", size); 
#endif
  return vmm.Allocate(size); 
}

void operator delete(void *ptr) { vmm.Free(ptr); }

void operator delete[](void *ptr) { vmm.Free(ptr); }
