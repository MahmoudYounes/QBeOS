#ifndef ASM_H
#define ASM_H

// contains all assembly definitions
#define RDMSR(addr, eax, edx) __asm__ __volatile__("rdmsr": "=a"(eax), "=d"(edx): "c"(addr))
#define WRMSR(addr, eax, edx) __asm__ __volatile__("wrmsr": : "c"(addr), "a"(eax), "d"(edx))

#endif /* ASM_H */
