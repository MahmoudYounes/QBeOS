#include "common.h"

inline void outb(uint16_t port, uint8_t val){
    __asm__ __volatile__ ("outb %0, %1"
                  :
                  : "dN" (port), "a" (val));
}

inline uint8_t inb(uint16_t port) {
    uint8_t retval;
    __asm__ __volatile__ ("inb %0, %1"
                          : "=r" (retval)
                          : "dN" (port));
    return retval;
}

inline void cli() {
    __asm__ __volatile__ ("cli");
}

inline void sti() {
    __asm__ __volatile__ ("sti");
}
