/*
 * List of common functionalities across the kernel
 *
 * 01010101101010001010011110101011
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

inline void outb(uint16_t port, uint8_t val);
inline uint8_t inb(uint16_t port);
inline void cli();
inline void sti();


#endif /* COMMON_H */
