#include "arch/x86/include/interrupt_32.h"

EXCEPTION BadInterrupt(struct interruptFrame *hwRegs) {
  panic("Interrupt is not implemented...\nhalting...\0");
}

EXCEPTION DivZero(struct interruptFrame *hwRegs) {
  kprint("Div By Zero Error happened\n\0");
  panic("Unsupported interrupt");

  // TODO: Terminate current running task on cpu
  // getCurrentCPUTask()
  // terminateTask()
}

EXCEPTION DebugException(struct interruptFrame *hwRegs) {
  kprint("QBeKern does not support debug exception yet.. continuing...\n\0");
  panic("Unsupported interrupt");
}

EXCEPTION NMI(struct interruptFrame *hwRegs) {
  kprint("QBeKern does not support NMI yet");
  panic("Unsupported interrupt");
}

EXCEPTION Breakpoint(struct interruptFrame *hwRegs) {
  kprint("QBeKern does not support breakpoints yet");
  panic("Unsupported interrupt");
}

EXCEPTION Overflow(struct interruptFrame *hwRegs) {
  kprint("Overflow Error happened\n\0");
  panic("Unsupported interrupt");
}

EXCEPTION BoundRangeExceeded(struct interruptFrame *hwRegs) {
  kprint("Exceeded bounds\n\0");
  panic("Unsupported interrupt");
}

EXCEPTION InvalidOpcode(struct interruptFrame *hwRegs) {
  kprint("Invalid opcode");
  panic("Unsupported interrupt");
}

EXCEPTION DeviceNotAbailable(struct interruptFrame *hwRegs) {
  kprint("FPU is not available");
  panic("Unsupported interrupt");
}

EXCEPTION DoubleFault(struct interruptFrame *hwRegs) {
  kprint("Double fault\n\0");
  panic("Unsupported interrupt");
}

EXCEPTION InvalidTSS(struct interruptFrame *hwRegs) {
  kprint("Invalid TSS");
  panic("Unsupported interrupt");
}

EXCEPTION SegmentNotPresent(struct interruptFrame *hwRegs) {
  kprint("Segment not present");
  panic("Unsupported interrupt");
}

EXCEPTION StackSegmentFault(struct interruptFrame *hwRegs) {
  kprint("Stack segment fault");
  panic("Unsupported interrupt");
}

EXCEPTION GeneralProtectionFault(struct interruptFrame *hwRegs) {
  kprint("General protection fault");
  panic("Unsupported interrupt");
}

EXCEPTION PageFault(struct interruptFrame *hwRegs) {
  kprint("Page fault");
  panic("Unsupported interrupt");
}

EXCEPTION MathFault(struct interruptFrame *hwRegs) {
  kprint("Math fault");
  panic("Unsupported interrupt");
}

EXCEPTION AligmentCheckFault(struct interruptFrame *hwRegs) {
  kprint("Alignment check fault");
  panic("Unsupported interrupt");
}

EXCEPTION MachineCheckFault(struct interruptFrame *hwRegs) {
  kprint("Machine check fault");
  panic("Unsupported interrupt");
}

EXCEPTION SIMDFault(struct interruptFrame *hwRegs) {
  kprint("SIMD fault");
  panic("Unsupported interrupt");
}

EXCEPTION VirtualizationFault(struct interruptFrame *hwRegs) {
  kprint("Virtualization fault");
  panic("Unsupported interrupt");
}

EXCEPTION ControlProtectionException(struct interruptFrame *hwRegs) {
  kprint("Control protection exception");
  panic("Unsupported interrupt");
}
