#ifndef MEMORY_H
#define MEMORY_H

#include "screen.h"
#include "mem_region.h"

class Memory{
    public:

        const static int countRegions = 1024;
        volatile int availableRegions = 0;
        MemoryRegion memoryRegions[countRegions];

        unsigned long *memoryTableAddress = (unsigned long *)0x70000;

        Memory();
        void PrintMemory();
};


#endif /* MEMORY_H */
