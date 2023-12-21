#ifndef MEM_REGION_H
#define MEM_REGION_H

#include "screen.h"

extern Screen screen;

enum memType{
    invalid,
    usable,
    reserved,
    acpiReclaim,
    acpiNVS,
    badMem
};

// class that holds physical memory region info
class MemoryRegion{
     public:
        unsigned long *baseAddress;
        unsigned long size;
        unsigned long id;
        enum memType type;

        long GetSize();
        void PrintRegionInfo();
};



#endif /* MEM_REGION_H */
