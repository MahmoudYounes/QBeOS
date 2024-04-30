#include "arch/x86/include/interrupt_32.h"

DEFINE_INTERRUPT(BadInterrupt) {
  panic("Interrupt is not implemented...\nhalting...\0");
}

DEFINE_INTERRUPT(DivZero) {
  panic("Div By Zero in kernel. unrecoverable...\n\0");
}

DEFINE_INTERRUPT(DebugException) {
  panic("Debug exception is not supported. check docs debug.md\n\0");
}

DEFINE_INTERRUPT(NMI) {
  panic("Non maskable interrupts are not implemented yet\n\0");
}

DEFINE_INTERRUPT(Breakpoint) {
  panic("Breakpoint interrupts are not supported. check docs debug.md\n\0");
}

DEFINE_INTERRUPT(Overflow) { panic("Unsupported interrupt: overflow\n\0"); }

DEFINE_INTERRUPT(BoundRangeExceeded) {
  panic("Unsupported interrupt: out of bounds\n\0");
}

DEFINE_INTERRUPT(InvalidOpcode) {
  panic("Unsupported interrupt: invalid opcode\n\0");
}

DEFINE_INTERRUPT(DeviceNotAbailable) {
  panic("Unsupported interrupt: device not available\n\0");
}

DEFINE_EXCEPTION(DoubleFault) {
  panic("Unsupported interrupt: Double fault\n\0");
}

DEFINE_EXCEPTION(InvalidTSS) {
  panic("Unsupported interrupt: invalid tss\n\0");
}

DEFINE_EXCEPTION(SegmentNotPresent) {
  panic("Unsupported interrupt: segment not present\n\0");
}

DEFINE_EXCEPTION(StackSegmentFault) {
  panic("Unsupported interrupt: stack segment fault\n\0");
}

DEFINE_EXCEPTION(GeneralProtectionFault) {
  panic("Unsupported interrupt: general protection fault\n\0");
}

DEFINE_EXCEPTION(PageFault) { panic("Unsupported interrupt: page fault\n\0"); }

DEFINE_INTERRUPT(MathFault) { panic("Unsupported interrupt: math fault\n\0"); }

DEFINE_EXCEPTION(AligmentCheckFault) {
  panic("Unsupported interrupt: Alignment check\n\0");
}

DEFINE_INTERRUPT(MachineCheckFault) {
  panic("Unsupported interrupt: machine check fault\n\0");
}

DEFINE_INTERRUPT(SIMDFault) { panic("Unsupported interrupt: SIMD fault\n\0"); }

DEFINE_INTERRUPT(VirtualizationFault) {
  panic("Unsupported interrupt: Virtualization fault\n\0");
}

DEFINE_EXCEPTION(ControlProtectionException) {
  panic("Unsupported interrupt: control protection\n\0");
}

DEFINE_INTERRUPT(TimerHandler) {
  kprint("timer called\n\0");
  apic.WriteEOI();
}

DEFINE_INTERRUPT(LINT0Handler) {
  kprint("LINT0 called\n\0");
  panic("can't proceed\n\0");
}

DEFINE_INTERRUPT(LINT1Handler) {
  kprint("LINT1 Called\n\0");
  panic("can't proceed\n\0");
}

DEFINE_INTERRUPT(PMCHandler) {
  kprint("PMC Called\n\0");
  panic("can't proceed\n\0");
}

DEFINE_INTERRUPT(CMCIHandler) {
  kprint("CMCI called\n\0");
  panic("can't proceed\n\0");
}

DEFINE_INTERRUPT(SpuriousHandler) {
  kprint("Spurious interrupt occured\n\0");
  panic("can't proceed\n\0");
}

DEFINE_INTERRUPT(APICErrHandler) {
  kprint("Error interrupt\n\0");
  panic("can't proceed\n\0");
}
