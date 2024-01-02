#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"
#include "screen.h"
#include "mem_region.h"
#include "strings.h"
#include "logger.h"

// 4 KB physical pages
#define PHYSICAL_PAGE_SIZE 4096
#define MEMORY_LIST_EXPECTED_SIZE_MBS 64
#define MEMORY_LIST_EXPECTED_SIZE_BYTES (MEMORY_LIST_EXPECTED_SIZE_MBS << 20)
#define KERNEL_MEMORY_REGION_SIZE_MBS 8 // TODO: figure out a way to dynamically calculate this
#define KERNEL_MEMORY_REGION_SIZE_BYTES (KERNEL_MEMORY_REGION_SIZE_MBS << 20)

class Memory{
    private:
        // Number of memory regions with size PHYSICAL_PAGE_SIZE
        uint64_t regionsCount = 0;

        // Address of the begining of memory regions list
        MemoryRegion *memoryListHead = (MemoryRegion *)0x500000;

        // Address of memory info created by bootloader
        uint32_t *memoryTableAddress = (uint32_t *)0x70000;

        // Pointer to first free Physical Page
        MemoryRegion *freePagePtr;

        // Keeps track of number of allocation requests.
        // TODO: if you ever get an md5 encryption, generate a checksum
        // to avoid hitting int64 boundary limit.
        uint64_t nextAllocID = 0;

        // Validates the memory list size is the size expected
        void assertMemoryListSize();

        // Parse memory region data and split into physical pages
        // returns the size of parsed region.
        uint64_t processMemoryTableEntry(uint64_t entry);

        // Splits the memory region into physical pages
        void splitRegion(const uint64_t regionBaseAddr,
                     const uint64_t size,
                     const uint32_t state,
                     const uint64_t bootMemRegionsCount);

        // Marks the memory regions of the kernel as used
        void reserverKernelMemory();

        // Sets the free page pointer to the first free page
        void setupFreePagePtr();

        // Given a memory region to free, free all memory regions
        // with the same allocation id as the given one.
        void freePageString(MemoryRegion *start);

        // Searches for a contigious region of pages, return a pointer
        // to the first available page in the region
        MemoryRegion *findEmptyRegionFor(uint64_t pages);

     public:
        Memory();
        void PrintMemory();
        void *AllocPhysicalPage();
        void *Allocate(uint64_t sizeBytes);
        void Free(void *pageAddr);
};


#endif /* MEMORY_H */
