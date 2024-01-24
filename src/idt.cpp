#include "idt.h"

extern Screen screen;

IDTEntry::IDTEntry(){}

IDTEntry::IDTEntry(uint64_t entry){
    offsetLow = entry & 0xffff;
    entry >>= 16;
    segment = entry & 0xffff;
    entry >>= 16;
    flags = entry & 0xff00;
    entry >>= 16;
    offsetHigh = entry & 0xffff;
    entry >>= 16;
    if (entry != 0){
        panic("entry not zero\n\0");
    }
}

void IDTEntry::SetOffset(uint32_t offset){
    offsetLow = offset & 0xffff;
    offsetHigh = offset >> 16;
}

void IDTEntry::SetSegment(uint16_t segment){
    IDTEntry::segment = segment;
}

void IDTEntry::SetFlags(uint16_t flags){
    IDTEntry::flags = flags;
}

uint32_t IDTEntry::GetOffset(){
    uint32_t offset = offsetHigh;
    offset <<= 16;
    offset = offset | offsetLow;
    return offset;
}

uint16_t IDTEntry::GetSegment(){
    return segment;
}

uint16_t IDTEntry::GetFlags(){
    return flags;
}


uint64_t IDTEntry::EncodeEntryAt(uintptr_t addr){
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

IDT::IDT(){
    setupIDT();
    loadIDT();
}

void IDT::setupIDT(){

}

void IDT::loadIDT(){

}
