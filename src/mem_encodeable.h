#ifndef MEM_ENTRY_H
#define MEM_ENTRY_H

#include "common.h"
#include "logger.h"

class MemoryEncodeable{
    public:
        virtual uint64_t EncodeEntryAt(uintptr_t addr)=0;
};

#endif /* MEM_ENTRY_H */
