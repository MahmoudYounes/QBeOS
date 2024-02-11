#include "interrupt_32.h"


INTERRUPT UnImplementedISR(struct interruptFrame *hwRegs){
    panic("Interrupt is not implemented...\nhalting...\0");
}
