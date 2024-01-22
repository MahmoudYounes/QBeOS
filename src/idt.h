#ifndef IDT_H
#define IDT_H

#include "mem_encodeable.h"
#include "common.h"

class IDTEntry : public MemoryEncodeable {
    public:
        uint16_t offsetLow;
        uint16_t segment;
        uint16_t flags;
        uint16_t offsetHigh;
        uint64_t EncodeEntryAt(uintptr_t addr);
};

class IDT {
    private:
        uintptr_t idtTableBase = 0x910000;
    public:
        IDT();

};


#endif /* IDT_H */
