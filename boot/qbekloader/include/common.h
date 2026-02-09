#ifndef COMMON_H
#define COMMON_H
#include <stdint.h>

#define NULL 0

static inline void outb(uint16_t port, uint8_t val){
    __asm__ __volatile__ ("out %0, %1"
                  :
                  : "dN" (port), "a" (val));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t retval;
    __asm__ __volatile__ ("in %0, %1"
                          : "=a" (retval)
                          : "dN" (port));
    return retval;
}

static inline void outl(uint16_t port, uint32_t val){
    __asm__ __volatile__ ("out %0, %1"
                  :
                  : "dN" (port), "a" (val)); 
}

static inline uint32_t inl(uint16_t port){
    uint32_t retval;
    __asm__ __volatile__ ("in %0, %1"
                          : "=a" (retval)
                          : "dN" (port));
  return retval;
}

static inline void cli() {
    __asm__ __volatile__ ("cli");
}

static inline void sti() {
    __asm__ __volatile__ ("sti");
}


static inline int max(int a, int b){
  if (a > b) return a;
  return b;
}

#define HLT() for(;;) __asm__("hlt")

void panic(char str[]);

#endif
