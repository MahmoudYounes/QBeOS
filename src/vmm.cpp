#include "vmm.h"
#include "common.h"
#include "formater.h"
#include "mem_region.h"
#include "pdt_entry.h"
#include "pt_entry.h"


static char *buf;

void VirtualMemory::testVirtualMemory(){
    screen.WriteString("should test memory?\0");
    screen.WriteString(shouldTestMemoryBeforePaging ? " YES\n\0":" NO\n\0");
    if (!shouldTestMemoryBeforePaging){
        return;
    }

    screen.WriteString("Hi\n\0");
    for (uintptr_t i = 0x0; i < 0xffffffff;i += 4){
        testAddrTranslation(i, PDTAddress);
    }
}

void VirtualMemory::testAddrTranslation(uintptr_t expectedAddr, uintptr_t PDTAddress){
    printf(buf, "vmm test: resolving vmm mapping for %p\n\0", expectedAddr);
    uint64_t firstTen = expectedAddr >> 22;
    uint64_t nextTen = (expectedAddr & 0x3ff000) >> 12;
    uint64_t lastTwelve = expectedAddr & 0xfff;

    uint32_t *pdtPtr = ((uint32_t *)PDTAddress + firstTen);
    uint32_t pdtEntry = *pdtPtr;

    uintptr_t ptPtr = pdtEntry & 0xfffff000;
    uint32_t *ptePtr = (uint32_t *)ptPtr + nextTen;
    uintptr_t pteContent = *ptePtr;
    uintptr_t paddr = pteContent & 0xfffff000;
    uintptr_t addr = paddr | lastTwelve;

    if(addr != expectedAddr){
        printf(buf, "page tables are incorrectly set up. expected %p found %p\n\0", expectedAddr, addr);
        printf(buf, "offseting pdt %p with %x yielded \0", PDTAddress, firstTen);
        printf(buf, "%p\n\0", pdtPtr);
        printf(buf, "addresss of PT from PDT Entry is %p\n\0", ptPtr);
        printf(buf, "offseting pt %p with %x yielded \0", ptPtr, nextTen);
        printf(buf, "%p\n\0", ptePtr);
        printf(buf, "address of P from PT Entry is %p\n\0", paddr);
        printf(buf, "constructing address from Page Base %p, and offset %x in page yielded \0", paddr, lastTwelve);
        printf(buf, "%p\n\0", addr);
        panic("vmm test failed");
    }
}

void VirtualMemory::initializeMemory(){
    screen.WriteString("Initializing virtual memory module...\n\0");
    setupPageTables();
    mapKernHigherHalf();
    enablePaging();
    unmapKernLowerHalf();
}

VirtualMemory::VirtualMemory(){
    initializeMemory();
}

VirtualMemory::VirtualMemory(bool shouldTestMemory){
    shouldTestMemoryBeforePaging = shouldTestMemory;
    initializeMemory();
}

void VirtualMemory::setupPageTables(){
    buf = (char *)sysMemory.AllocPhysicalPage();

    MemoryInfo physicalMemInfo = sysMemory.GetMemoryInfo();
    uint32_t *pdtPtr = (uint32_t *)PDTAddress;
    uint32_t *ptPtr = (uint32_t *)PTAddress;
    uint64_t i = 0, j = 0, gc = 0;

    // creating identity page directory tables and page tables
    MemoryRegion *ptr = (MemoryRegion *)physicalMemInfo.pagesWalker;
    for(i = 0; i < ENTRIES_COUNT;i++){
        createPDTEntry((uintptr_t)pdtPtr, (uintptr_t) ptPtr);
        for(j = 0; j < ENTRIES_COUNT; j++){
            createPTEntry((uintptr_t)ptPtr, (uintptr_t)ptr->baseAddress, IS_SYS_REGION(ptr));

            ptr = ptr->next;
            gc++;
            ptPtr++;
        }
        pdtPtr++;
    }
}

void VirtualMemory::mapKernHigherHalf(){

}

void VirtualMemory::enablePaging(){
    // test translate if enabled
    testVirtualMemory();

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

void VirtualMemory::createPDTEntry(uintptr_t atPDTPtr, uintptr_t ofPTPtr){
    KERN_PDT->SetPTAddress(ofPTPtr)->EncodeEntryAt(atPDTPtr);
}

void VirtualMemory::createPTEntry(uintptr_t atPTPtr, uintptr_t ofPtr, bool isKern){
    if(isKern){
        KERN_PT->SetPageAddress(ofPtr)->EncodeEntryAt(atPTPtr);
    } else {
        // TODO: once we have IDT, lazy create these virual pages on page faults?
        USER_PT->SetPageAddress(ofPtr)->EncodeEntryAt(atPTPtr);
    }
}

VirtualMemory vmm;
