#ifndef STRINGS_H
#define STRINGS_H

#include "common.h"
#include "stddef.h"
#include "math.h"

// TODO: restructure
extern "C" void memcpy(void *dstpp, void *srcpp, size_t count);
extern "C" void memset(void *dstpp, uint8_t val, size_t count);
extern "C" uint64_t strlen(char *strp);
extern "C" int8_t strcmp(char *str1p, char *str2p);
#endif /* STRINGS_H */
