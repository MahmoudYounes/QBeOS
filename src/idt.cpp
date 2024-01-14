#include "idt.h"

void IDTEntry::SetOffset(uint32_t offset){
    offsetLow = offset & 0xffff;
    offsetHigh = offset >> 16;
}

void IDTEntry::SetSegment(uint16_t segment){
    segment = segment;
}

void IDTEntry::SetFlags(uint16_t flags){
    flags = flags;
}

uint64_t IDTEntry::EncodeEntryAt(uintptr_t addr){
    uint32_t offsetAndSegment = ((uint32_t)segment << 16) | offsetLow;
    uint32_t offsetAndFlags = ((uint32_t)offsetHigh << 16) | flags;
    *(uint32_t *)addr = offsetAndSegment;
    *((uint32_t *)addr + 1) = offsetAndFlags;
    return *(uint64_t *)addr;
}

IDT::IDT(){
    setupIDT();
    loadIDT();
}

void IDT::setupIDT(){

}

void IDT::loadIDT(){

}
