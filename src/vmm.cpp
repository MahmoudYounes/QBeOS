#include "vmm.h"
#include "common.h"
#include "mem_region.h"

VirtualMemory::VirtualMemory(){
    setupPageTables();
    mapSystemMemory();
    enablePaging();
    unmapSystemMemory();
}

void VirtualMemory::setupPageTables(){
    MemoryInfo *physicalMemInfo = sysMemory.GetMemoryInfo();
    uintptr_t ptPtr = PTAddress;

    // creating identity page tables
    MemoryRegion *ptr = (MemoryRegion *)physicalMemInfo->pagesWalker;
    while(ptr->next != NULL){
        createPTEntry(ptPtr, ptr);

        ptPtr += ENTRY_SIZE_BYTES / WORD_SIZE;
        ptr = ptr->next;
    }
}

void VirtualMemory::mapSystemMemory(){

}

void VirtualMemory::enablePaging(){
    // after paging is enabled, must do a far jump to the next kernl address.
    // how the hell am I going to calculate that?
}

void VirtualMemory::unmapSystemMemory(){

}

void VirtualMemory::createPTEntry(uintptr_t ptPtr, MemoryRegion *memPtr){
}
