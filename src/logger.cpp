#include "logger.h"

extern Screen screen;

void panic(const char * errMsg){
    screen.WriteString("panic: ");
    screen.WriteString(errMsg);
    asm("hlt");
}

void printInfo(const char *msg){
    screen.WriteString("info: ");
    screen.WriteString(msg);
    screen.WriteString("\n");
}
