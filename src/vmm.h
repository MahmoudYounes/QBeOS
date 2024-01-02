#ifndef VMM_H
#define VMM_H

#include "common.h"
#include "pdt_entry.h"
#include "pt_entry.h"


class VirtualMemory{
    // PDT address, check OSMap.txt
    static const uintptr_t PDTAddress = 0x2500000;

    // Set up the page directory tables and page tables
    void setupPageTables();

    // Identity maps the lower kern memory and system memory
    void mapSystemMemory();

    // Unmap the lower kern memory and system memory from lower pages
    void unmapSystemMemory();

    // Enables paging
    void enablePaging();

    public:
        VirtualMemory();
};


#endif /* VMM_H */
