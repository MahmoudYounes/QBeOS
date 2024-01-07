#include "logger.h"

extern Screen screen;

void panic(const char * errMsg){
    screen.WriteString("panic: \0");
    screen.WriteString(errMsg);
    asm("hlt");
}

void printInfo(const char *msg){
    screen.WriteString("info: ");
    screen.WriteString(msg);
    screen.WriteString("\n");
}
