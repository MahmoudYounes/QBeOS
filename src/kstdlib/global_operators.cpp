#include "kstdlib/include/global_operators.h"

void *operator new(size_t size) { return vmm.Allocate(size); }

void *operator new[](size_t size) { return vmm.Allocate(size); }

void operator delete(void *ptr) { vmm.Free(ptr); }

void operator delete[](void *ptr) { vmm.Free(ptr); }
