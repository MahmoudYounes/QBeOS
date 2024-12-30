#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "include/common.h"

// contains all assembly definitions
#define IA32_SYSENTER_CS 0x174
#define RDMSR(addr, eax, edx) __asm__ __volatile__("rdmsr": "=a"(eax), "=d"(edx): "c"(addr))
#define WRMSR(addr, eax, edx) __asm__ __volatile__("wrmsr": : "c"(addr), "a"(eax), "d"(edx))


// X86 registers types defined here. these types should change based on whether we are compiling
// 32bit or 64bit kernel.
// TODO: implement 64bit kernel
typedef uint16_t segmentreg_size_t;
typedef uint32_t generalreg_size_t;
typedef uint32_t controlreg_size_t;

extern "C" generalreg_size_t readIP();


#endif /* PROCESSOR_H */
