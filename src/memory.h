#ifndef MEMORY_H
#define MEMORY_H

#include "screen.h"

enum memType{
invalid,
usable,
reserved,
acpiReclaim,
acpiNVS,
badMem
};

class MemoryRegion{
    public:
        unsigned long *baseAddress;
        unsigned long length;
        enum memType type;

        long GetSize();

};

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
