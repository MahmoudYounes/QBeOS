#include "include/logger.h"

void panic(const char *errMsg) {
  screen.WriteString("[KERN] panic: \0");
  screen.WriteString(errMsg);
  asm("hlt");
}

void kprint(const char *msg) {
  screen.WriteString("[KERN] ");
  screen.WriteString(msg);
}
