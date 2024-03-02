/*
 * List of common functionalities across the kernel
 * TODO: this is a bad practice. Once completed, this file should be split
 * into multiple files with specific functionalities and descriptive
 * namings
 **/

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdalign.h>
#include <stdarg.h>
#include <stddef.h>

// when switching to 64 bits this should be read as config
#define WORD_SIZE 32
#define SIZE_OF_WORD sizeof(uint32_t)
#define BITS_PER_BYTE 8

#define CONCAT_INTS(low, high) (high << 4) | low
#define GET_BYTE(val, n)  (val >> (n*8)) & 0xff
#define BYTE_TO_KB(val) (val) >> 10
#define BYTE_TO_MB(val) (val) >> 20
#define BYTE_TO_GB(val) (val) >> 30
#define GB_TO_BYTE(val) ((uint64_t)val) << 30
#define MB_TO_BYTE(val) ((uint64_t)val) << 20
#define KB_TO_BYTE(val) ((uint64_t)val) << 10

#define MAX_B_SIZE() 1 << 10
#define MAX_KB_SIZE() 1 << 20
#define MAX_MB_SIZE() 1 << 30
#define MAX_GB_SIZE() 1 << 40

#define GET_LOWER_WORD(val) val & 0xffffffff
#define GET_HIGHER_WORD(val) val & 0xffffffff00000000
#define SET_LOWER_WORD(var, val) var | val
#define SET_HIGHER_WORD(var, val) var | ((uint64_t)val << WORD_SIZE)

#define HLT() for(;;) asm("hlt")

// TODO: <todo> this code should go into arch folder
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
// </todo>
#endif /* COMMON_H */
