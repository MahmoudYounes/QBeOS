#ifndef MEM_REGION_H
#define MEM_REGION_H

#include "screen.h"
#include "common.h"

extern Screen screen;

enum memState{
    invalid,
    usable,
    reserved,
    acpiReclaim,
    acpiNVS,
    badMem,
    // any new types additions start here
    KERN,
};

// class that holds physical memory region info
class MemoryRegion{
     public:
        uint8_t *baseAddress;
        uint64_t size;
        uint64_t bootRegionID;
        uint64_t regionID;
        uint64_t allocRequestID = 0;
        enum memState state;
        MemoryRegion *next;

        long GetSize();
        void PrintRegionInfo();
};



#endif /* MEM_REGION_H */
