#ifndef LOGGER_H
#define LOGGER_H

#include "common.h"
#include "screen.h"
#include "formater.h"

extern Screen screen;

void panic(const char *msg);
void kprint(const char *msg);

#define sprintf(resBuf, str, ...) Formater().Format(resBuf, str, __VA_ARGS__)

// this leaks the screen variable into the file used in but since we only have one global screen it should be okay.
#define kprintf(resBuf, str, ...)                \
    do {                                                 \
    extern Screen screen;                                \
    sprintf(resBuf, "[KERN] " str, __VA_ARGS__);         \
    screen.WriteString(resBuf);                 \
    } while (0)

#endif
