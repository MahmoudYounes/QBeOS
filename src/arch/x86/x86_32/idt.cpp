#include "arch/x86/include/idt.h"

// interruptVector contains pointers to the vector methods that will be used
// TODO: a smarter way is to define the whole 256 entries and specify overrides!
static uintptr_t *interruptVector[] = {(uintptr_t *)DivZero,
                                       (uintptr_t *)DebugException,
                                       (uintptr_t *)NMI,
                                       (uintptr_t *)Breakpoint,
                                       (uintptr_t *)Overflow,
                                       (uintptr_t *)BoundRangeExceeded,
                                       (uintptr_t *)InvalidOpcode,
                                       (uintptr_t *)DeviceNotAbailable,
                                       (uintptr_t *)DoubleFault,
                                       (uintptr_t *)BadInterrupt,
                                       (uintptr_t *)InvalidTSS,
                                       (uintptr_t *)SegmentNotPresent,
                                       (uintptr_t *)StackSegmentFault,
                                       (uintptr_t *)GeneralProtectionFault,
                                       (uintptr_t *)PageFault,
                                       (uintptr_t *)BadInterrupt,
                                       (uintptr_t *)MathFault,
                                       (uintptr_t *)AligmentCheckFault,
                                       (uintptr_t *)MachineCheckFault,
                                       (uintptr_t *)SIMDFault,
                                       (uintptr_t *)VirtualizationFault,
                                       (uintptr_t *)ControlProtectionException,
                                       (uintptr_t *)BadInterrupt,
                                       (uintptr_t *)BadInterrupt,
                                       (uintptr_t *)BadInterrupt,
                                       (uintptr_t *)BadInterrupt,
                                       (uintptr_t *)BadInterrupt,
                                       (uintptr_t *)BadInterrupt,
                                       (uintptr_t *)BadInterrupt,
                                       (uintptr_t *)BadInterrupt,
                                       (uintptr_t *)BadInterrupt};

static int countDefinedInterrupts = 31;

IDTEntry::IDTEntry() {}

IDTEntry::IDTEntry(uint64_t entry) {
  offsetLow = entry & 0xffff;
  entry >>= 16;
  segment = entry & 0xffff;
  entry >>= 16;
  flags = entry & 0xff00;
  entry >>= 16;
  offsetHigh = entry & 0xffff;
  entry >>= 16;
  if (entry != 0) {
    panic("entry not zero\n\0");
  }
}

void IDTEntry::SetOffset(uint32_t offset) {
  offsetLow = offset & 0xffff;
  offsetHigh = offset >> 16;
}

void IDTEntry::SetSegment(uint16_t segment) { IDTEntry::segment = segment; }

void IDTEntry::SetFlags(uint16_t flags) { IDTEntry::flags = flags; }

uint32_t IDTEntry::GetOffset() {
  uint32_t offset = offsetHigh;
  offset <<= 16;
  offset = offset | offsetLow;
  return offset;
}

uint16_t IDTEntry::GetSegment() { return segment; }

uint16_t IDTEntry::GetFlags() { return flags; }

uint64_t IDTEntry::EncodeEntryAt(uintptr_t addr) {
  uint32_t *addrCasted = (uint32_t *)addr;

  uint32_t offsetAndSegment = 0;
  offsetAndSegment |= segment;
  offsetAndSegment <<= 16;
  offsetAndSegment |= offsetLow;

  uint32_t offsetAndFlags = 0;
  offsetAndFlags |= offsetHigh;
  offsetAndFlags <<= 16;
  offsetAndFlags |= flags;

  *addrCasted = offsetAndSegment;
  *(addrCasted + 1) = offsetAndFlags;
  return *(uint64_t *)addr;
}

IDT::IDT() {
  // interrupts should be set up only once
  uint8_t idtIdx = 0;
  for (int i = 0; i < countDefinedInterrupts; i++) {
    IDTEntry idtEntry = IDTEntry();
    idtEntry.SetSegment(GDT_KERNEL_CODE_DESCRIPTOR_SEL);
    idtEntry.SetOffset((uintptr_t)interruptVector[i]);
    idtEntry.SetFlags(GATE_32INTR_F);
    idtEntry.EncodeEntryAt(idtTableBase + idtIdx++ * 8);
  }

  idt.offset = idtTableBase;
  idt.size = 8 * countDefinedInterrupts;

  __asm__ __volatile__("lidt %0" : : "m"(idt));
}

IDT idt;
