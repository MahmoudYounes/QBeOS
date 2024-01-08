#ifndef CXA_PURE_VIRTUAL_H
#define CXA_PURE_VIRTUAL_H

#include "common.h"
#include "screen.h"

extern Screen screen;

// Needs to be provided for linker when using pure virtual functions.
// This function is called if a member derived from a class does not implement
// an abstract method.
extern "C" void __cxa_pure_virtual() {
    screen.WriteString("error in virutal function pointers");
    HLT();
}

#endif /* CXA_PURE_VIRTUAL_H */
