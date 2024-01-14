#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"
#include "screen.h"
#include "mem_region.h"
#include "strings.h"
#include "logger.h"
#include "formater.h"
#include "math.h"

// 4 KB physical pages
#define PHYSICAL_PAGE_SIZE 4096
// Expected size of Physical Memory Table size
#define MEMORY_LIST_EXPECTED_SIZE_MBS 32
#define MEMORY_LIST_EXPECTED_SIZE_BYTES MEMORY_LIST_EXPECTED_SIZE_MBS << 20
// Size of memory reserved by the kernel
#define KERNEL_MEMORY_REGION_SIZE_MBS 64 // TODO: figure out a way to dynamically calculate this
#define KERNEL_MEMORY_REGION_SIZE_BYTES (KERNEL_MEMORY_REGION_SIZE_MBS << 20)

struct MemoryInfo{
        uint64_t memSizeBytes;
        uintptr_t pagesWalker;
        uint64_t pagesCount;
};

struct MemTableEntry{
        uint64_t baseAddr;
        uint64_t size;
        uint32_t state;
};


class Memory{
    private:
        // Number of memory regions with size PHYSICAL_PAGE_SIZE
        uint64_t physicalPagesCount = 0;

        // Address of the begining of memory regions list
        MemoryRegion *memoryListHead = (MemoryRegion *)0x920000;

        // Address of memory info created by bootloader
        uint32_t *memoryTableAddress = (uint32_t *)0x70000;

        // Pointer to first free Physical Page
        MemoryRegion *freePagePtr;

        // Keeps track of number of allocation requests.
        // TODO: if you ever get an md5 encryption, generate a checksum
        // to avoid hitting int64 boundary limit.
        uint64_t nextAllocID = 0;

        // Keeps Metadata about physical memory cached
        MemoryInfo memInfo;

        // Validates the memory list size is the size expected
        void assertMemoryListSize();

        // Splits the memory region into physical pages
        void splitRegion(const MemTableEntry *mtentry,
                     const uint64_t bootMemRegionIdx);

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
        MemoryInfo GetMemoryInfo();
        MemoryRegion GetPageAt(uintptr_t paddr);
};

#endif /* MEMORY_H */
