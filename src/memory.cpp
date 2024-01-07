#include "memory.h"
#include "mem_region.h"

extern Screen screen;

void Memory::assertMemoryListSize() {
    uint64_t memoryListSizeBytes =  (memoryListHead + physicalPagesCount) - memoryListHead;
    if (memoryListSizeBytes * sizeof(MemoryRegion) >= MEMORY_LIST_EXPECTED_SIZE_BYTES){
        panic("memory tables overflow\n");
    }
}

void Memory::splitRegion(const MemTableEntry *mtentry, uint64_t bootMemRegionIdx){
    MemoryRegion bufRegion = MemoryRegion();
    MemoryRegion *currMemListPointer = memoryListHead + physicalPagesCount;
    uint64_t i = 0;
    for (; i < (mtentry->size / PHYSICAL_PAGE_SIZE); i++){
        bufRegion.baseAddress = (uint8_t *)(mtentry->baseAddr + i * PHYSICAL_PAGE_SIZE);
        bufRegion.size = PHYSICAL_PAGE_SIZE;
        bufRegion.state = (enum memState)mtentry->state;
        bufRegion.bootRegionID = bootMemRegionIdx;
        bufRegion.regionID = physicalPagesCount;
        bufRegion.next = currMemListPointer + 1; // the next Memory region would be here.

        memcpy(currMemListPointer, &bufRegion, sizeof(MemoryRegion));

        currMemListPointer++;
        physicalPagesCount++;
    }

    // a left over memory here means the memory can't be contigious
    // this is because a state change from usable to something else implies
    // the next memory region is not usable. the only usable memory is the one
    // marked explicitly as usable.
    if (mtentry->size % PHYSICAL_PAGE_SIZE != 0){
        bufRegion.baseAddress = (uint8_t *)(mtentry->baseAddr + i * PHYSICAL_PAGE_SIZE);
        bufRegion.size = PHYSICAL_PAGE_SIZE;
        bufRegion.state = (enum memState)KERN;
        bufRegion.bootRegionID = bootMemRegionIdx;
        bufRegion.regionID = physicalPagesCount++;
        bufRegion.next = currMemListPointer + 1; // the next Memory region would be here.
        memcpy(currMemListPointer, &bufRegion, sizeof(MemoryRegion));
    }
}

void Memory::reserverKernelMemory(){
    // the first 8MBs are always reserved for the kernel code and data
    uint64_t sizeToReserver= KERNEL_MEMORY_REGION_SIZE_BYTES;
    MemoryRegion *ptr = memoryListHead;
    uint64_t actualReserved = 0;
    while (sizeToReserver > 0) {
        if (ptr->state != reserved){
            ptr->state = KERN;
            actualReserved += ptr->GetSize();
        }

        ptr->allocRequestID = 0;
        sizeToReserver -= ptr->GetSize();
        ptr = ptr->next;
    }
    screen.WriteString("reserved 8MBs for kernel of which \0");
    screen.WriteInt(BYTE_TO_MB(actualReserved));
    screen.WriteString(" MBs are reserved as KERN and \0");
    screen.WriteInt(BYTE_TO_KB(KERNEL_MEMORY_REGION_SIZE_BYTES - actualReserved));
    screen.WriteString(" KBs are reserved for system...\n\0");
}

void Memory::setupFreePagePtr(){
    MemoryRegion *ptr = memoryListHead;
    while(ptr->state != usable){
        ptr = ptr->next;
    }

    freePagePtr = ptr;
}

MemoryRegion *Memory::findEmptyRegionFor(uint64_t pages){
    MemoryRegion *ptr = freePagePtr;
    MemoryRegion *walker = ptr;
    uint64_t countScannedPages = pages;
    while(countScannedPages > 0 && walker != NULL){
        if (walker->state == usable){
            countScannedPages--;
            walker++;
        } else {
            countScannedPages = pages;
            ptr = walker;
            walker = ptr;
        }
    }
    if (countScannedPages != 0) {
        panic("failed to find a contigious area of memory");
    }
    return ptr;
}

Memory::Memory(){
    char buf[4096];
    screen.WriteString("Initializing memory...\n");
    uint64_t bootMemRegionsCount = 0;
    uint64_t endAddr = 0;

    do{
        // TODO: sometimes int15h eax e820 returns 24 bytes instead of 20 bytes.
        // this needs to be passed as argument to the kernel from boot loader.

        // every memory region record is 20 or 24 bytes
        // base address in 8 bytes, size in 8 bytes, type in 4 bytes, & ACPI Attr. 4 bytes
        MemTableEntry mtentry;
        memcpy(&mtentry, memoryTableAddress + bootMemRegionsCount * 5, sizeof(mtentry));

        if(mtentry.size != 0 && endAddr != mtentry.baseAddr){
            MemTableEntry memHole = {
                .baseAddr = endAddr,
                .size = mtentry.baseAddr - endAddr,
                .state = reserved
            };
            splitRegion(&memHole, bootMemRegionsCount);
            uint64_t memHoleEndAddr = memHole.baseAddr + memHole.size;
            printf(buf, "found memory hole. start: %x end: at %x\n\0", memHole.baseAddr, memHoleEndAddr);
        }

        if (mtentry.size != 0) {
            splitRegion(&mtentry, bootMemRegionsCount);
        }

        endAddr = mtentry.baseAddr + mtentry.size;

        printf(buf, "found memory region. start: %x end: at %x\n\0", mtentry.baseAddr, endAddr);

        // This is not a reliable exit condition.
        if (mtentry.size == 0){
            break;
        }
        bootMemRegionsCount++;
    } while(1);

    memoryListHead[physicalPagesCount - 1].next = NULL;
    for (uint64_t i = 0; i < physicalPagesCount; i++){
        if((i != physicalPagesCount - 1 && memoryListHead[i].next == NULL) ||
           (i == physicalPagesCount - 1 && memoryListHead[i].next != NULL)){
            screen.WriteString("failed to set list pointers correctly at \0");
            screen.WriteInt(i);
            screen.WriteString("\n\0");
            panic("none");
        }
    }

    int i = 0;
    MemoryRegion *ptr = memoryListHead;
    while(ptr->next->next != NULL){
        if(ptr->next->baseAddress - ptr->baseAddress != 4096){
            panic("memory is not sorted");
        }
        ptr = ptr->next;
        i++;
    }
    screen.WriteString("walked the memory \0");
    screen.WriteInt(i+1);
    screen.WriteString("times \n\0");

    reserverKernelMemory();
    setupFreePagePtr();
    nextAllocID = 1;

    uint64_t memSizeBytes = 0;
    for (MemoryRegion *ptr = memoryListHead; ptr != NULL; ptr = ptr->next){
        memSizeBytes += ptr->GetSize();
    }

    memInfo = MemoryInfo{
        .memSizeBytes = memSizeBytes,
        .pagesWalker = (uintptr_t)memoryListHead,
        .pagesCount = physicalPagesCount,
    };
}

void Memory::PrintMemory(){
    uint64_t availableMemory = 0;
    uint64_t reservedMemory = 0;
    for (MemoryRegion *ptr = memoryListHead; ptr != NULL; ptr = ptr->next){
        if (ptr->state == usable) {
            availableMemory += ptr->GetSize();
        } else {
            reservedMemory += ptr->GetSize();
        }
    }

    screen.WriteString("System ram size is ");
    screen.WriteInt(BYTE_TO_MB(memInfo.memSizeBytes));
    screen.WriteString("MBs.\n");

    screen.WriteString("Usable memory: ");
    screen.WriteInt(BYTE_TO_MB(availableMemory));
    screen.WriteString("MBs.\n");

    screen.WriteString("Reserved memory: ");
    screen.WriteInt(BYTE_TO_KB(reservedMemory));
    screen.WriteString("KBs.\n");
}

void *Memory::AllocPhysicalPage(){
    return Allocate(PHYSICAL_PAGE_SIZE);
}

void *Memory::Allocate(uint64_t sizeBytes){
    if (sizeBytes == 0)
        return NULL;

    nextAllocID++;
    uint64_t numPagesToAllocate;
    if (sizeBytes % PHYSICAL_PAGE_SIZE > 0)
        numPagesToAllocate = (sizeBytes + PHYSICAL_PAGE_SIZE) / PHYSICAL_PAGE_SIZE;
    else
        numPagesToAllocate = sizeBytes / PHYSICAL_PAGE_SIZE;

    MemoryRegion *beginAddress = findEmptyRegionFor(numPagesToAllocate);

    volatile MemoryRegion *ptr = beginAddress;
    for(volatile uint64_t i = 0; i < numPagesToAllocate; i++){
        if (ptr->state != usable){
            panic("trying to allocate reserved page in contigious region");
        }

        ptr->state = KERN;
        ptr->allocRequestID = nextAllocID;
        ptr = ptr->next;
    }

    setupFreePagePtr();
    return beginAddress->baseAddress;
}

void Memory::Free(void *pageAddr){
    if (pageAddr == NULL)
        return;

    uint8_t *addr = (uint8_t *)pageAddr;
    MemoryRegion *ptr = memoryListHead;
    while(ptr != NULL){
        if (addr == ptr->baseAddress){
            freePageString(ptr);
            setupFreePagePtr();
            return;
        }

        ptr = ptr->next;
    }
    panic("trying to free an incorrect address");
}


void Memory::freePageString(MemoryRegion *start){
    uint64_t allocID = start->allocRequestID;
    while (start != NULL && start->allocRequestID == allocID) {
        if (start->state != KERN){
            panic("trying to free an unreserved page");
        }

        start->state = usable;
        start->allocRequestID = 0;
        start = start->next;
    }
}

MemoryInfo Memory::GetMemoryInfo(){
    return memInfo;
}

// Global Memory Variable;
Memory sysMemory;
