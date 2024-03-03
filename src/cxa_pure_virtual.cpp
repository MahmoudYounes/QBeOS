#ifndef CXA_PURE_VIRTUAL_H
#define CXA_PURE_VIRTUAL_H

#include "include/common.h"
#include "include/logger.h"

// Needs to be provided for linker when using pure virtual functions.
// This function is called if a member derived from a class does not implement
// an abstract method.
extern "C" void __cxa_pure_virtual() {
  panic("error in virutal function pointers");
}

#endif /* CXA_PURE_VIRTUAL_H */
