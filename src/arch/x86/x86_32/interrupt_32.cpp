#include "arch/x86/include/interrupt_32.h"
#include "arch/x86/include/pic.h"

void BadInterrupt9(struct interruptFrame *hwregs){
  panic("Interrupt 9 is not implemented...\nhalting...\0");
}

void BadInterrupt15(struct interruptFrame *hwregs){
  panic("Interrupt 15 is not implemented...\nhalting...\0");
}

void BadInterrupt22(struct interruptFrame *hwregs){
  panic("Interrupt 22 is not implemented...\nhalting...\0");
}

void BadInterrupt23(struct interruptFrame *hwregs){
  panic("Interrupt 23 is not implemented...\nhalting...\0");
}

void BadInterrupt24(struct interruptFrame *hwregs){
  panic("Interrupt 24 is not implemented...\nhalting...\0");
}

void BadInterrupt25(struct interruptFrame *hwregs){
  panic("Interrupt 25 is not implemented...\nhalting...\0");

}

void BadInterrupt26(struct interruptFrame *hwregs){
panic("Interrupt 26 is not implemented...\nhalting...\0");

}

void BadInterrupt27(struct interruptFrame *hwregs){
panic("Interrupt 27 is not implemented...\nhalting...\0");

}

void BadInterrupt28(struct interruptFrame *hwregs){
panic("Interrupt 28 is not implemented...\nhalting...\0");

}

void BadInterrupt29(struct interruptFrame *hwregs){
panic("Interrupt 29 is not implemented...\nhalting...\0");

}

void BadInterrupt30(struct interruptFrame *hwregs){
panic("Interrupt 30 is not implemented...\nhalting...\0");

}

void BadInterrupt31(struct interruptFrame *hwregs){
panic("Interrupt 31 is not implemented...\nhalting...\0");

}

void BadInterrupt38(struct interruptFrame *hwregs){
panic("Interrupt 38 is not implemented...\nhalting...\0");

}

void BadInterrupt39(struct interruptFrame *hwregs){
panic("Interrupt 39 is not implemented...\nhalting...\0");

}

void BadInterrupt40(struct interruptFrame *hwregs){
panic("Interrupt 40 is not implemented...\nhalting...\0");

}

void BadInterrupt41(struct interruptFrame *hwregs){
panic("Interrupt 41 is not implemented...\nhalting...\0");

}

void BadInterrupt42(struct interruptFrame *hwregs){
panic("Interrupt 42 is not implemented...\nhalting...\0");

}

void BadInterrupt43(struct interruptFrame *hwregs){
panic("Interrupt 43 is not implemented...\nhalting...\0");

}

void BadInterrupt44(struct interruptFrame *hwregs){
panic("Interrupt 44 is not implemented...\nhalting...\0");

}

void BadInterrupt45(struct interruptFrame *hwregs){
panic("Interrupt 45 is not implemented...\nhalting...\0");

}

void BadHRDWRInterrupt(struct interruptFrame *hwregs){
  panic("Bad hardware interrupt\n\0");  
};

void DivZero(struct interruptFrame *hwregs){
  panic("Div By Zero in kernel. unrecoverable...\n\0");
}

void DebugException(struct interruptFrame *hwregs){
  panic("Debug exception is not supported. check docs debug.md\n\0");
}

void NMI(struct interruptFrame *hwregs){
  panic("Non maskable interrupts are not implemented yet\n\0");
}

void Breakpoint(struct interruptFrame *hwregs){
  panic("Breakpoint interrupts are not supported. check docs debug.md\n\0");
}

void Overflow(struct interruptFrame *hwregs){ panic("Unsupported interrupt: overflow\n\0"); }

void BoundRangeExceeded(struct interruptFrame *hwregs){
  panic("Unsupported interrupt: out of bounds\n\0");
}

void InvalidOpcode(struct interruptFrame *hwregs){
  panic("Unsupported interrupt: invalid opcode\n\0");
}

void DeviceNotAbailable(struct interruptFrame *hwregs){
  panic("Unsupported interrupt: device not available\n\0");
}

void DoubleFault(struct interruptFrame *hwregs, uint32_t errCode){
  panic("Unsupported interrupt: Double fault\n\0");
}

void InvalidTSS(struct interruptFrame *hwregs, uint32_t errCode){
  panic("Unsupported interrupt: invalid tss\n\0");
}

void SegmentNotPresent(struct interruptFrame *hwregs, uint32_t errCode){
  panic("Unsupported interrupt: segment not present\n\0");
}

void StackSegmentFault(struct interruptFrame *hwregs, uint32_t errCode){
  panic("Unsupported interrupt: stack segment fault\n\0");
}

void GeneralProtectionFault(struct interruptFrame *hwregs, uint32_t errCode){
  uint32_t isExternal, idt, ti;
  uint32_t descriptorIndex;

  isExternal = errCode & 1;
  idt = errCode & 2;
  ti = errCode & 4;

  descriptorIndex = errCode & ~0x7; 

  kprint("panic: general protection fault\n\0");
  kprintf("fault codes are isExternal: %d isIDT: %d is isTI: %d\n\0", isExternal, idt, ti);
  kprintf("descriptor index: %d\n\0", descriptorIndex);

  panic("stopping execution\n\0");
}

void PageFault(struct interruptFrame *hwregs, uint32_t errCode){ 
  panic("Unsupported interrupt: page fault\n\0"); 
}

void MathFault(struct interruptFrame *hwregs){ 
  panic("Unsupported interrupt: math fault\n\0"); 
}

void AligmentCheckFault(struct interruptFrame *hwregs, uint32_t errCode){
  panic("Unsupported interrupt: Alignment check\n\0");
}

void MachineCheckFault(struct interruptFrame *hwregs){
  panic("Unsupported interrupt: machine check fault\n\0");
}

void SIMDFault(struct interruptFrame *hwregs){ 
  panic("Unsupported interrupt: SIMD fault\n\0"); 
}

void VirtualizationFault(struct interruptFrame *hwregs){
  panic("Unsupported interrupt: Virtualization fault\n\0");
}

void ControlProtectionException(struct interruptFrame *hwregs, uint32_t errCode){
  panic("Unsupported interrupt: control protection\n\0");
}

void TimerHandler(struct interruptFrame *hwregs){
  kprint("timer called\n\0");
  apic.WriteEOI();
}

void LINT0Handler(struct interruptFrame *hwregs){ 
  kprint("LINT0 called\n\0"); 
}

void LINT1Handler(struct interruptFrame *hwregs){ 
  kprint("LINT1 Called\n\0"); 
}

void PMCHandler(struct interruptFrame *hwregs){ 
  kprint("PMC Called\n\0"); 
}

void CMCIHandler(struct interruptFrame *hwregs){ 
  kprint("CMCI called\n\0"); 
}

void SpuriousHandler(struct interruptFrame *hwregs){ 
  kprint("Spurious interrupt occured\n\0"); 
}

void APICErrHandler(struct interruptFrame *hwregs){ 
  kprint("Error interrupt\n\0"); 
}

void PITTimerHandler(struct interruptFrame *hwregs){
  kprint("handling timer interrupt\n\0"); 
  pit.Reload();
  pic.SendEOI(0);
}

void KeyboardHandler(struct interruptFrame *hwregs){
  kprint("recieved keyboard interrupt\n\0");
  pic.SendEOI(1); 
}
