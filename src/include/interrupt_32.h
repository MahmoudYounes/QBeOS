/** ALWAYS ALWAYS WORK WITH THE COMPILER NOT AGAINST IT */

#ifndef INTERRUPT_32_H
#define INTERRUPT_32_H

// This file defines the interrupt vector methods

#include "common.h"
#include "logger.h"


#define INTERRUPT void __attribute__((interrupt))
#define EXCEPTION void __attribute__((interrupt))

// interruptFrame is the frame used to hold register values. this is setup by the compiler.
struct interruptFrame{
    uint16_t ss;
    uint32_t esp;
    uint16_t eflags;
    uint16_t cs;
    uint32_t eip;
};


// definitions of all functions here. implementation is found in cpp file
EXCEPTION BadInterrupt(struct interruptFrame *hwRegs);
EXCEPTION DivZero(struct interruptFrame *hwRegs);
EXCEPTION DebugException(struct interruptFrame *hwRegs);
EXCEPTION NMI(struct interruptFrame *hwRegs);
EXCEPTION Breakpoint(struct interruptFrame *hwRegs);
EXCEPTION Overflow(struct interruptFrame *hwRegs);
EXCEPTION BoundRangeExceeded(struct interruptFrame *hwRegs);
EXCEPTION InvalidOpcode(struct interruptFrame *hwRegs);
EXCEPTION DeviceNotAbailable(struct interruptFrame *hwRegs);
EXCEPTION DoubleFault(struct interruptFrame *hwRegs);
EXCEPTION InvalidTSS(struct interruptFrame *hwRegs);
EXCEPTION SegmentNotPresent(struct interruptFrame *hwRegs);
EXCEPTION StackSegmentFault(struct interruptFrame *hwRegs);
EXCEPTION GeneralProtectionFault(struct interruptFrame *hwRegs);
EXCEPTION PageFault(struct interruptFrame *hwRegs);
EXCEPTION MathFault(struct interruptFrame *hwRegs);
EXCEPTION AligmentCheckFault(struct interruptFrame *hwRegs);
EXCEPTION MachineCheckFault(struct interruptFrame *hwRegs);
EXCEPTION SIMDFault(struct interruptFrame *hwRegs);
EXCEPTION VirtualizationFault(struct interruptFrame *hwRegs);
EXCEPTION ControlProtectionException(struct interruptFrame *hwRegs);
#endif /* INTERRUPT_32_H */
