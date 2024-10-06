#include "include/logger.h"

void panic(const char *errMsg) {
  screen.WriteString("[KERN] panic: \0");
  screen.WriteString(errMsg);
  for (;;) asm("hlt");
}

void kprint(const char *msg) {
  screen.WriteString("[KERN] ");
  screen.WriteString(msg);
}

/**
* scan all %es to calculate the size of the result buffer.
* put all

void kprintf(const char *msg, ...){
  char *fstr;

}
*/
