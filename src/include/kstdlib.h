#ifndef KSTDLIB_H
#define KSTDLIB_H

#include "include/common.h"
#include "include/vmm.h"

extern VirtualMemory vmm;

void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *ptr);
void operator delete[](void *ptr);

#endif /* KSTDLIB_H */
