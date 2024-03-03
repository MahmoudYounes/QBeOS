#ifndef STRINGS_H
#define STRINGS_H

#include "common.h"
#include "math.h"
#include "stddef.h"

// TODO: restructure
extern "C" void memcpy(void *dstpp, void *srcpp, size_t count);
extern "C" void memset(void *dstpp, uint8_t val, size_t count);
extern "C" uint64_t strlen(const char *strp);
extern "C" int8_t strcmp(const char *str1p, const char *str2p);
#endif /* STRINGS_H */
