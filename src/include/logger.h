#ifndef LOGGER_H
#define LOGGER_H

#include "arch/include/screen.h"
#include "common.h"
#include "formater.h"

extern Screen screen;

void __attribute__((noreturn)) panic(const char *msg);
void kprint(const char *msg);

#define sprintf(resBuf, str, ...) Formater().Format(resBuf, str, __VA_ARGS__)

// this leaks the screen variable into the file used in but since we only have
// one global screen it should be okay.
#define kprintf(str, ...)                                                      \
  do {                                                                         \
    extern Screen screen;                                                      \
    static char buf[4096];                                                     \
    sprintf(buf, "[KERN] " str, __VA_ARGS__);                                  \
    screen.WriteString(buf);                                                   \
  } while (0)

#endif
