#ifndef STRINGS_H
#define STRINGS_H

#include "common.h"
#include "stddef.h"

// TODO: restructure
extern "C" void memcpy(void *dstpp, void *srcpp, size_t count);
extern "C" void memset(void *dstpp, uint8_t val, size_t count);

#endif /* STRINGS_H */
