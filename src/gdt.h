#ifndef GDT_H
#define GDT_H

#include "common.h"
#include "logger.h"
#include "screen.h"

struct GDTEntry {
    uint32_t limit;
    uint32_t base;
    uint8_t access;
    uint8_t flags;
};


class GDT {
    private:
        static const uint32_t gdtBaseAddress = 0x00400000;
        uint8_t *lastEntryAddress;
        uint8_t countEntries = 0;

        void encodeEntry(GDTEntry *entry);

        int tmp = 0;
    public:
        GDT();

        GDTEntry ConstructGDTEntry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);

        // TODO: how are you going to think about error propagation?
        void AddGDTEntry(GDTEntry *entry);

        void RefreshGDT();
};


#endif /* GDT_H */
