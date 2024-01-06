#include "vmm.h"
#include "common.h"
#include "formater.h"
#include "mem_region.h"
#include "pdt_entry.h"
#include "pt_entry.h"

VirtualMemory::VirtualMemory(){
    screen.WriteString("Initializing virtual memory module...\n\0");
    setupPageTables();
    mapKernHigherHalf();
    enablePaging();
    unmapKernLowerHalf();
}

void VirtualMemory::setupPageTables(){
    MemoryInfo physicalMemInfo = sysMemory.GetMemoryInfo();
    uintptr_t pdtPtr = PDTAddress;
    uintptr_t ptPtr = PTAddress;

    // creating identity page directory tables and page tables
    MemoryRegion *ptr = (MemoryRegion *)physicalMemInfo.pagesWalker;
    uint64_t i = 0;
    while(ptr->next != NULL){
        createPDTEntry(pdtPtr, ptPtr); // PDT -> PT
        createPTEntry(ptPtr, ptr);     // PT -> P

        pdtPtr += ENTRY_SIZE_BYTES / WORD_SIZE;
        ptPtr += ENTRY_SIZE_BYTES / WORD_SIZE;
        ptr = ptr->next;

        i++;
        if (i >= physicalMemInfo.pagesCount){
            panic("exceeded memory physical pages count \n\0");
        }
    }
}

void VirtualMemory::mapKernHigherHalf(){

}

void VirtualMemory::enablePaging(){
    screen.WriteString("Enabling paging, no return...\n\0");
    // TODO: if we want higher half kernel then after paging is enabled, must do a far jump to the next kernl address
    __asm__ __volatile__ (
        "mov ecx, %0\n\t"
        "mov cr3, ecx\n\t"
        "xor ecx, ecx\n\t"
        "mov ecx, cr0\n\t"
        "or ecx, 0x80000001\n\t"
        "mov cr0, ecx\n\t"
        :
        : "r"(PDTAddress));
}

void VirtualMemory::unmapKernLowerHalf(){

}

void VirtualMemory::createPDTEntry(uintptr_t pdtPtr, uintptr_t ptPtr){
    KERN_PDT->SetPTAddress(ptPtr)->EncodeEntryAt(pdtPtr);
}

void VirtualMemory::createPTEntry(uintptr_t ptPtr, MemoryRegion *memPtr){
    if(IS_SYS_REGION(memPtr)){
        KERN_PT->SetPageAddress((uintptr_t)memPtr->baseAddress)->EncodeEntryAt(ptPtr);
    } else {
        // TODO: once we have IDT, lazy create these virual pages on page faults?
        USER_PT->SetPageAddress((uintptr_t)memPtr->baseAddress)->EncodeEntryAt(ptPtr);
    }
}

VirtualMemory vmm;
