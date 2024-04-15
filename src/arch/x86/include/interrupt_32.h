/** ALWAYS ALWAYS WORK WITH THE COMPILER NOT AGAINST IT */

#ifndef INTERRUPT_32_H
#define INTERRUPT_32_H

// This file defines the interrupt vector methods

#include "include/common.h"
#include "include/logger.h"

#define DEFINE_INTERRUPT(interruptName) void __attribute__((interrupt)) interruptName(struct interruptFrame *hwRegs)
#define DEFINE_EXCEPTION(exceptionName) void __attribute__((interrupt)) exceptionName(struct interruptFrame *hwRegs, uint32_t errCode)
// interruptFrame is the frame used to hold register values. this is setup by
// the compiler.
struct interruptFrame {
  uint16_t ss;
  uint32_t esp;
  uint16_t eflags;
  uint16_t cs;
  uint32_t eip;
};

// definitions of all functions here. implementation is found in cpp file
DEFINE_INTERRUPT(BadInterrupt);
DEFINE_INTERRUPT(DivZero);
DEFINE_INTERRUPT(DebugException);
DEFINE_INTERRUPT(NMI);
DEFINE_INTERRUPT(Breakpoint);
DEFINE_INTERRUPT(Overflow);
DEFINE_INTERRUPT(BoundRangeExceeded);
DEFINE_INTERRUPT(InvalidOpcode);
DEFINE_INTERRUPT(DeviceNotAbailable);
DEFINE_EXCEPTION(DoubleFault);
DEFINE_EXCEPTION(InvalidTSS);
DEFINE_EXCEPTION(SegmentNotPresent);
DEFINE_EXCEPTION(StackSegmentFault);
DEFINE_EXCEPTION(GeneralProtectionFault);
DEFINE_EXCEPTION(PageFault);
DEFINE_INTERRUPT(MathFault);
DEFINE_EXCEPTION(AligmentCheckFault);
DEFINE_INTERRUPT(MachineCheckFault);
DEFINE_INTERRUPT(SIMDFault);
DEFINE_INTERRUPT(VirtualizationFault);
DEFINE_EXCEPTION(ControlProtectionException);
#endif /* INTERRUPT_32_H */
