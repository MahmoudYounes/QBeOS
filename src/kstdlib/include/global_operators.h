#ifndef KSTDLIB_H
#define KSTDLIB_H

#include "arch/include/vmm.h"
#include "include/common.h"

extern VirtualMemory vmm;

void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *ptr);
void operator delete[](void *ptr);

#endif /* KSTDLIB_H */
