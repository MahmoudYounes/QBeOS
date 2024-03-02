#ifndef IDT_H
#define IDT_H

#include "mem_encodeable.h"
#include "common.h"
#include "interrupt_32.h"

#define GATE_32INTR_F 0x8e00      // 1P 00PL 0R  e(32bit intr gate) 00R

class IDTEntry : public MemoryEncodeable {
    private:
        uint16_t offsetLow;
        uint16_t segment;
        uint16_t flags; // MSB -> LSB: Present,2 * DPL, Res 0, 4 * GateType, 8 * Res
        uint16_t offsetHigh;
    public:
        IDTEntry();
        IDTEntry(uint64_t entry);
        uint64_t EncodeEntryAt(uintptr_t addr);
        void SetOffset(uint32_t offset);
        void SetSegment(uint16_t segment);
        void SetFlags(uint16_t flags);
        uint32_t GetOffset();
        uint16_t GetSegment();
        uint16_t GetFlags();
};

class IDT {
    private:
        uintptr_t idtTableBase = 0x910000;

    public:
        struct __attribute__((packed)) IDTInfo {
                uint16_t size;
                uint32_t offset;
        } idt;
        IDT();

};


#endif /* IDT_H */
