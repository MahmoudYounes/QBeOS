#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

inline void outb(uint16_t port, uint8_t val);

inline uint8_t inb(uint16_t port);

#endif /* COMMON_H */
