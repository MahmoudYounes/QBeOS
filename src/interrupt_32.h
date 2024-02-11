/** ALWAYS ALWAYS WORK WITH THE COMPILER NOT AGAINST IT */

#ifndef INTERRUPT_32_H
#define INTERRUPT_32_H

// This file defines the interrupt vector methods

#include "common.h"
#include "screen.h"
#include "formater.h"
#include "logger.h"


#define INTERRUPT void __attribute__((interrupt))
#define EXCEPTION void __attribute__((exception))

// interruptFrame is the frame used to hold register values. this is setup by the compiler.
struct interruptFrame{
    uint16_t ss;
    uint32_t esp;
    uint16_t eflags;
    uint16_t cs;
    uint32_t eip;
};


// definitions of all functions here. implementation is found in cpp file
INTERRUPT UnImplementedISR(struct interruptFrame *hwRegs);


#endif /* INTERRUPT_32_H */
