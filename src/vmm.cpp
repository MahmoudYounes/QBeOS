#include "vmm.h"
#include "memory.h"

static char *buf;

void VirtualMemory::testVirtualMemory(){
    if (!shouldTestMemoryBeforePaging){
        return;
    }

    for (uintptr_t i = 0x0; i < 0xffffffff;i += 4){
        testAddrTranslation(i);
    }
}

void VirtualMemory::testAddrTranslation(uintptr_t expectedAddr){
    uint64_t firstTen = expectedAddr >> 22;
    uint64_t nextTen = (expectedAddr >> 12) & 0x3ff;
    uint64_t lastTwelve = expectedAddr & 0xfff;

    uintptr_t pdtPtr = PDTAddress + firstTen * ENTRY_SIZE_BYTES;
    uint32_t pdtEntry = *(uint32_t *)pdtPtr;

    uintptr_t ptPtr = (pdtEntry >> 12) << 12;
    uintptr_t ptePtr = ptPtr + nextTen * ENTRY_SIZE_BYTES;
    uintptr_t pteContent = *(uint32_t *)ptePtr;
    uintptr_t paddr = (pteContent >> 12) << 12;
    uintptr_t addr = paddr | lastTwelve;

    if(addr != expectedAddr){
        printf(buf, "testing mapping of address %p\n\0", expectedAddr);
        printf(buf, "page tables are incorrectly set up. expected %p found %p\n\0", expectedAddr, addr);
        printf(buf, "offseting pdt %p with %x yielded \0", PDTAddress, firstTen);
        printf(buf, "%p\n\0", pdtPtr);
        printf(buf, "addresss of PT from PDT Entry is %p\n\0", ptPtr);
        printf(buf, "offseting pt %p with %d yielded \0", ptPtr, nextTen);
        printf(buf, "%p\n\0", ptePtr);
        printf(buf, "address of Page from PT Entry is %p\n\0", paddr);
        printf(buf, "constructing address from Page Base %p, and offset %d in page yielded \0", paddr, lastTwelve);
        printf(buf, "%p\n\0", addr);
        panic("vmm test failed");
    }
}

void VirtualMemory::initializeMemory(){
    print("Initializing virtual memory module...\n\0");
    setupPageDirectoryTable();
    mapSystemMemory();
    enablePaging();
}

VirtualMemory::VirtualMemory(){
    initializeMemory();
}

VirtualMemory::VirtualMemory(bool shouldTestMemory){
    shouldTestMemoryBeforePaging = shouldTestMemory;
    initializeMemory();
}

void VirtualMemory::setupPageDirectoryTable(){
    for(int i = 0; i < ENTRIES_COUNT; i++){
        uintptr_t pdtAddr = PDTAddress + i * ENTRY_SIZE_BYTES;
        uintptr_t ptAddr = PTAddress + i * ENTRIES_COUNT * ENTRY_SIZE_BYTES;
        createPDTEntry(pdtAddr, ptAddr);
    }
}

void VirtualMemory::mapSystemMemory(){
    MemoryInfo physicalMemInfo = sysMemory.GetMemoryInfo();
    // creating identity page directory tables and page tables
    MemoryRegion *ptr = (MemoryRegion *)physicalMemInfo.pagesWalker;
    while(ptr != NULL){
        if (ptr->state != usable){
            uint32_t flags = 0;
            if (ptr->state == KERN)
                flags |= VMM_KERN;
            if (ptr->state == reserved)
                flags |= VMM_RESV;
            map((uintptr_t)ptr->baseAddress, (uintptr_t)ptr->baseAddress, flags);
        }
        ptr = ptr->next;
    }

    // mapping video memory
    map(0xb8000, 0xb8000, VMM_KERN);
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

void VirtualMemory::createPDTEntry(uintptr_t atPDTPtr, uintptr_t ofPTPtr){
    KERN_PDT->SetPTAddress(ofPTPtr)->EncodeEntryAt(atPDTPtr);
}

void VirtualMemory::createPTEntry(uintptr_t atPTPtr, uintptr_t ofPtr, uint8_t flags){
    if(flags & VMM_KERN){
        KERN_PT->SetPageAddress(ofPtr)->EncodeEntryAt(atPTPtr);
    } else if (flags & VMM_RESV || flags & VMM_UNMAP){
        INVALID_PT.SetPageAddress(ofPtr)->EncodeEntryAt(atPTPtr);
    } else {
        // TODO: once we have IDT, lazy create these virual pages on page faults?
        USER_PT->SetPageAddress(ofPtr)->EncodeEntryAt(atPTPtr);
    }
}

void VirtualMemory::map(uintptr_t vaddr, uintptr_t paddr, uint8_t flags){
    uint64_t pdtOffset = vaddr >> 22;
    uint64_t ptOffset = (vaddr >> 12) & 0x3ff;
    uintptr_t physicalPageBeginAddr = paddr & ~0xfff;
    uintptr_t pdtAddr = PDTAddress + pdtOffset * ENTRY_SIZE_BYTES;
    uintptr_t ptAddr = ((*(uint32_t *)pdtAddr) >> 12) << 12;
    uintptr_t ptEntryAddr = ptAddr + ptOffset * ENTRY_SIZE_BYTES;

    createPTEntry((uintptr_t)ptEntryAddr, physicalPageBeginAddr, flags);
    flushTLB();
}

uint32_t VirtualMemory::GetPageEntry(uintptr_t vaddr){
    uint64_t pdtOffset = vaddr >> 22;
    uint64_t ptOffset = (vaddr >> 12) & 0x3ff;
    uintptr_t pdtAddr = PDTAddress + pdtOffset * ENTRY_SIZE_BYTES;
    uintptr_t ptAddr = ((*(uint32_t *)pdtAddr) >> 12) << 12;
    uintptr_t ptEntryAddr = ptAddr + ptOffset * ENTRY_SIZE_BYTES;
    return *(uint32_t*)ptEntryAddr;
}


// TODO: there are two ways to implement this. Either allocate the entire memory
// physically then virtually map it, or allocate page by page. when you have
// time tracers evaluate which is better.
void *VirtualMemory::Allocate(size_t size){
    if (size == 0){
        // hathzar hanhazar
        return NULL;
    }

    uint32_t numPages;
    if (size % PHYSICAL_PAGE_SIZE != 0){
        numPages = (size + PHYSICAL_PAGE_SIZE) / PHYSICAL_PAGE_SIZE; // math.ciel
    } else {
        numPages = size / PHYSICAL_PAGE_SIZE;
    }

    void *allocatedPage = (void *)sysMemory.AllocPhysicalPage();
    for(uint32_t i = 0; i < numPages-1; i++){
        uintptr_t pptr = (uintptr_t)sysMemory.AllocPhysicalPage();
        map(pptr, pptr, VMM_KERN);
    }
    return (void *)allocatedPage;
}

void VirtualMemory::unmap(uintptr_t vaddr){
    uint64_t pdtOffset = vaddr >> 22;
    uint64_t ptOffset = (vaddr >> 12) & 0x3ff;
    uintptr_t pdtAddr = PDTAddress + pdtOffset * ENTRY_SIZE_BYTES;
    uintptr_t ptAddr = ((*(uint32_t *)pdtAddr) >> 12) << 12;
    uintptr_t ptEntryAddr = ptAddr + ptOffset * ENTRY_SIZE_BYTES;

    createPTEntry((uintptr_t)ptEntryAddr, 0, VMM_UNMAP);
}

void VirtualMemory::Free(void *ptr){
    uintptr_t paddr = virtualToPhysicalAddr((uintptr_t)ptr);
    uintptr_t vptr = (uintptr_t)ptr;
    MemoryRegion startPage = sysMemory.GetPageAt(paddr);
    uint64_t allocId = startPage.allocRequestID;

    for(MemoryRegion *walker = &startPage; walker != NULL; walker = walker->next){
        unmap(vptr);
        if (walker->allocRequestID != allocId){
            break;
        }

        vptr += PHYSICAL_PAGE_SIZE;
        walker = walker->next;
    }
    sysMemory.Free((void *)paddr);
    flushTLB();
}

uintptr_t VirtualMemory::virtualToPhysicalAddr(uintptr_t vaddr){
    uintptr_t paddr;
    uint32_t ent = GetPageEntry(vaddr);
    paddr = (ent >> 12) << 12;
    return paddr;
}

// TODO: Consider implementing the signle version of TLB flush
void VirtualMemory::flushTLB(){
    __asm__ __volatile(
        "mov eax, cr3\n\t"
        "mov cr3, eax\n\t");
}

VirtualMemory vmm;
