#ifndef KSTDLIB_H
#define KSTDLIB_H

#include "include/configs.h"
#include "arch/include/vmm.h"
#include "include/logger.h"

void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *ptr);
void operator delete[](void *ptr);

#endif /* KSTDLIB_H */
