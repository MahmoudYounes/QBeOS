/*
 * List of common functionalities across the kernel
 *
 **/

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdint.h>
#include <stdalign.h>

// when switching to 64 bits this should be read as config
#define WORD_SIZE = 32;

#define CONCAT_INTS(low, high) (high << 4) | low;
#define GET_BYTE(val, n)  (val >> (n*8)) & 0xff
#define BYTE_TO_KB(val) val >> 10
#define BYTE_TO_MB(val) val >> 20
#define BYTE_TO_GB(val) val >> 30

#define KB_SIZE() 1024 << 10
#define MB_SIZE() 1024 << 20
#define GB_SIZE() 1024 << 30

#define HLT() for(;;) asm("hlt")

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

#endif /* COMMON_H */
