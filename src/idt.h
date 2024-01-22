#ifndef IDT_H
#define IDT_H

#include "mem_encodeable.h"
#include "common.h"

class IDTEntry : public MemoryEncodeable {

    private:
        uint16_t offsetLow;
        uint16_t segment;
        uint16_t flags;
        uint16_t offsetHigh;
    public:
        uint64_t EncodeEntryAt(uintptr_t addr);
        void SetOffset(uint32_t offset);
        void SetSegment(uint16_t segment);
        void SetFlags(uint16_t flags);
};

class IDT {
    private:
        uintptr_t idtTableBase = 0x910000;

        // Loads the IDT
        void loadIDT();

        // Sets up the IDT
        void setupIDT();

    public:
        IDT();

};


#endif /* IDT_H */
