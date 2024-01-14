#ifndef VMM_H
#define VMM_H

#include "common.h"
#include "pdt_entry.h"
#include "pt_entry.h"

#include "memory.h"

#define ENTRIES_COUNT 1024
#define ENTRY_SIZE_BYTES sizeof(uint32_t)
#define TABLE_SIZE_BYTES ENTRIES_COUNT * ENTRY_SIZE_BYTES

// flags for mapping
#define VMM_KERN  1
#define VMM_RESV  2

extern Memory sysMemory;
extern Screen screen;

class VirtualMemory{
    // PDT address, check OSMap.txt
    // When moving to 64 bits PDTs will exist.
    static const uintptr_t PDTAddress = 0x500000;
    static const uintptr_t PTAddress =  0x510000;
    bool shouldTestMemoryBeforePaging = false;

    // Implementing unit tests for paging
    void testVirtualMemory();
    void testAddrTranslation(uintptr_t expectedAddr);

    // Runs vmm initialization routine
    void initializeMemory();

    // Set up the page directory table
    void setupPageDirectoryTable();

    // Maps the system kernel memory
    void mapSystemMemory();

    // creates a virtual page directory table entry and encodes it
    void createPDTEntry(uintptr_t pdtPtr, uintptr_t ptPtr);

    // Creates a virtual page entry and encodes it
    void createPTEntry(uintptr_t ptPtr, uintptr_t ofPtr, uint8_t flags);

    // Enables paging
    void enablePaging();

    public:
        VirtualMemory();
        VirtualMemory(bool shouldTestMemory);

        // Maps a virtual address to a physical address
        void Map(uintptr_t vaddr, uintptr_t paddr, uint8_t flags);

        // Allocates virtual memory. TODO: convert to lazy allocation
        void *Allocate(size_t size);

        uint32_t GetPageEntry(uintptr_t addr);

};


#endif /* VMM_H */
