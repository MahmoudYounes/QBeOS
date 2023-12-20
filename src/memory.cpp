#include "memory.h"
#include "screen.h"

#define CONCAT_INTS(low, high) (high << 4) | low

extern Screen screen;

Memory::Memory(){
    screen.WriteString("Initializing memory...\n");
    availableRegions = 0;
    do{
        // every memory region record is 5 bytes
        // base address in 2 bytes, size in 2 bytes, type in 1 byte

        long baseAddrl = memoryTableAddress[availableRegions * 5];
        long baseAddrh = memoryTableAddress[availableRegions * 5 + 1];
        long sizel = memoryTableAddress[availableRegions * 5 + 2];
        long sizeh = memoryTableAddress[availableRegions * 5 + 3];
        long type = memoryTableAddress[availableRegions * 5 + 4];

        long baseAddr = CONCAT_INTS(baseAddrl, baseAddrh);
        long size = CONCAT_INTS(sizel, sizeh);

        if (size == 0){
            break;
        }

        MemoryRegion newRegion = MemoryRegion();
        newRegion.baseAddress = (unsigned long *)baseAddr;
        newRegion.size = size;
        newRegion.type = (enum memType)type;

        memoryRegions[availableRegions++] = newRegion;
    } while(1);

}

void Memory::PrintMemory(){
    long memSizeBytes = 0;
    for (int i = 0; i < availableRegions; i++){
        memSizeBytes += memoryRegions[i].GetSize();
    }

    screen.WriteString("System ram size is ");
    screen.WriteIntToScreen(memSizeBytes / 1024 / 1024);
    screen.WriteString("MBs.\n");

    screen.WriteString("Memory regions: \n");
    for (int i = 0; i < availableRegions; i++){
        memoryRegions[i].PrintRegionInfo();
    }
}

// Global Memory Variable;
Memory sysMemory;
