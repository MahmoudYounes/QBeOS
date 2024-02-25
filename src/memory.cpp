#include "memory.h"
#include "common.h"
#include "math.h"
#include "mem_region.h"

static char buf[512];

void Memory::assertMemoryListSize() {
    uint64_t memoryListSizeBytes =  physicalPagesCount * sizeof(MemoryRegion);
    if (memoryListSizeBytes >= MEMORY_LIST_EXPECTED_SIZE_BYTES){
        panic("memory tables overflow\n");
    }
}

void Memory::splitRegion(const MemTableEntry *mtentry, uint64_t bootMemRegionIdx){
    MemoryRegion bufRegion = MemoryRegion();
    MemoryRegion *currMemListPointer = memoryListHead + physicalPagesCount;
    uint64_t i = 0;
    for (; i < (mtentry->size / PHYSICAL_PAGE_SIZE); i++){
        bufRegion.baseAddress = ((uint8_t *)mtentry->baseAddr + i * PHYSICAL_PAGE_SIZE);
        bufRegion.size = PHYSICAL_PAGE_SIZE;
        bufRegion.state = (enum memState)mtentry->state;
        bufRegion.bootRegionID = bootMemRegionIdx;
        bufRegion.regionID = physicalPagesCount;
        bufRegion.allocRequestID = 0;
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
        bufRegion.allocRequestID = 0;
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

        sizeToReserver -= ptr->GetSize();
        ptr = ptr->next;
    }

    kprintf(buf, "reserved %dMBs for kernel\n\0", BYTE_TO_MB(actualReserved));
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
    kprint("Initializing memory...\n");
    uint64_t bootMemRegionsCount = 0;
    uint64_t endAddr = 0;

    // memory tables structure has 4 bytes that contain the size of the table
    uint32_t countRegions = *memoryTableAddress;
    memoryTableAddress++;

    while(bootMemRegionsCount < countRegions){
        // TODO: sometimes int15h eax e820 returns 24 bytes instead of 20 bytes.
        // this needs to be passed as argument to the kernel from boot loader.

        // every memory region record is 20 or 24 bytes
        // base address in 8 bytes, size in 8 bytes, type in 4 bytes, & ACPI Attr. 4 bytes
        MemTableEntry mtentry;
        memcpy(&mtentry, memoryTableAddress + bootMemRegionsCount * 5, sizeof(mtentry));

        if(mtentry.size != 0 && endAddr != mtentry.baseAddr){
            mtentry = {
                .baseAddr = endAddr,
                .size = mtentry.baseAddr - endAddr,
                .state = reserved
            };
            uint64_t memHoleEndAddr = mtentry.baseAddr + mtentry.size;
            kprintf(buf, "found memory hole. start: %X end: at %X\n\0", mtentry.baseAddr, memHoleEndAddr);
            splitRegion(&mtentry, bootMemRegionsCount);

            endAddr = memHoleEndAddr;
            continue;
        }

        if (mtentry.size != 0) {
            splitRegion(&mtentry, bootMemRegionsCount);
        }

        endAddr = mtentry.baseAddr + mtentry.size;

        kprintf(buf, "found memory region. start: %X end: at %X\n\0", mtentry.baseAddr, endAddr);

        // This is not a reliable exit condition.
        if (mtentry.size == 0){
            break;
        }
        bootMemRegionsCount++;
    }

    memoryListHead[physicalPagesCount - 1].next = NULL;
    for (uint64_t i = 0; i < physicalPagesCount; i++){
        if((i != physicalPagesCount - 1 && memoryListHead[i].next == NULL) ||
           (i == physicalPagesCount - 1 && memoryListHead[i].next != NULL)){
            kprintf(buf, "failed to set list pointers correctly at %d\n\0", i);
            panic("memory self tests failed");
        }
    }

    int i = 0;
    MemoryRegion *ptr1 = memoryListHead;
    MemoryRegion *ptr2 = ptr1->next;
    while(ptr2 != NULL){
        if(ptr2->baseAddress - ptr1->baseAddress > 4096){
            panic("memory is not sorted");
        }
        ptr1 = ptr2;
        ptr2 = ptr2->next;
        i++;
    }

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
    uint64_t kernelMemory = 0;
    uint64_t reservedMemory = 0;
    for (MemoryRegion *ptr = memoryListHead; ptr != NULL; ptr = ptr->next){
        if (ptr->state == usable) {
            availableMemory += ptr->GetSize();
        } else if (ptr->state == KERN) {
            kernelMemory += ptr->GetSize();
        } else {
            reservedMemory += ptr->GetSize();
        }
    }

    kprintf(buf, "system ram: %d MBs.\n\0", BYTE_TO_MB(memInfo.memSizeBytes));
    kprintf(buf, "usable memory: %d MBs.\n\0", BYTE_TO_MB(availableMemory));
    kprintf(buf, "kernel memory: %d MBs.\n\0", BYTE_TO_MB(kernelMemory));
    kprintf(buf, "reserved memory: %d MBs.\n\0", BYTE_TO_MB(reservedMemory));
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

MemoryRegion Memory::GetPageAt(uintptr_t paddr){
    uint8_t *paddrPageAligned = (uint8_t *)(paddr & ~0xfff);
    int lidx = 0;
    int ridx = physicalPagesCount;
    int midIdx = (ridx - lidx) / 2;
    while(lidx < ridx && memoryListHead[midIdx].baseAddress != paddrPageAligned){
        if (memoryListHead[midIdx].baseAddress <= paddrPageAligned){
            // paddr on the right
            lidx = midIdx;
        } else {
            // paddr on the left
            ridx = midIdx;
        }

        midIdx = ((ridx - lidx) / 2) + lidx;
    }

    return memoryListHead[midIdx];
}

// Global Memory Variable;
Memory sysMemory;
