#ifndef VMM_H
#define VMM_H

#include "common.h"
#include "pdt_entry.h"
#include "pt_entry.h"
#include "memory.h"

#define ENTRIES_COUNT 1024
#define ENTRY_SIZE_BYTES sizeof(uint32_t)
#define TABLE_SIZE_BYTES ENTRIES_COUNT * ENTRY_SIZE_BYTES

extern Memory sysMemory;
extern Screen screen;

class VirtualMemory{
    // PDT address, check OSMap.txt
    // When moving to 64 bits PDTs will exist.
    static const uintptr_t PDTAddress = 0x2500000;
    static const uintptr_t PTAddress = PDTAddress + (sizeof(uint32_t) * ENTRIES_COUNT);

    // Set up the page directory tables and page tables
    void setupPageTables();

    // creates a virtual page directory table entry and encodes it
    void createPDTEntry(uintptr_t pdtPtr, uintptr_t ptPtr);

    // Creates a virtual page entry and encodes it
    void createPTEntry(uintptr_t ptPtr, uintptr_t ofPtr, bool isKern);

    // Identity maps the lower kern memory and system memory
    void mapKernHigherHalf();

    // Unmap the lower kern memory and system memory from lower pages
    void unmapKernLowerHalf();

    // Enables paging
    void enablePaging();

    public:
        VirtualMemory();
};


#endif /* VMM_H */
