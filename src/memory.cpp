#include "memory.h"
#include "common.h"
#include "mem_region.h"

extern Screen screen;


void Memory::assertMemoryListSize() {
    uint64_t memoryListSizeBytes =  (memoryListHead + regionsCount) - memoryListHead;
    if (memoryListSizeBytes * sizeof(MemoryRegion) >= MEMORY_LIST_EXPECTED_SIZE_BYTES){
        panic("memory tables overflow\n");
    }
}

uint64_t Memory::processMemoryTableEntry(uint64_t entry){
    // TODO: sometimes int15h eax e820 returns 24 bytes instead of 20 bytes.
    // this needs to be passed as argument to the kernel from boot loader.

    // every memory region record is 20 or 24 bytes
    // base address in 8 bytes, size in 8 bytes, type in 4 bytes, & ACPI Attr. 4 bytes
    uint32_t baseAddrl = memoryTableAddress[entry * 5];
    uint32_t baseAddrh = memoryTableAddress[entry * 5 + 1];
    uint32_t sizel =  memoryTableAddress[entry * 5 + 2];
    uint32_t sizeh = memoryTableAddress[entry * 5 + 3];
    uint32_t state = memoryTableAddress[entry * 5 + 4];

    uint64_t size = 0;
    size = SET_LOWER_WORD(size, sizel);
    size = SET_HIGHER_WORD(size, sizeh);
    if (size == 0) {
        return size;
    }

    uint64_t baseAddr = 0;
    baseAddr = SET_LOWER_WORD(baseAddr, baseAddrl);
    baseAddr = SET_HIGHER_WORD(baseAddr, baseAddrh);

    splitRegion(baseAddr, size, state, entry);

    return size;
}

void Memory::splitRegion(const uint64_t regionBaseAddr,
                         uint64_t size,
                         uint32_t state,
                         uint64_t bootMemRegionsCount){
    MemoryRegion bufRegion = MemoryRegion();
    MemoryRegion *currMemListPointer = memoryListHead + regionsCount;
    uint64_t i = 0;
    for (; i < (size / PHYSICAL_PAGE_SIZE); i++){
        bufRegion.baseAddress = (uint8_t *)(regionBaseAddr + i * PHYSICAL_PAGE_SIZE);
        bufRegion.size = PHYSICAL_PAGE_SIZE;
        bufRegion.state = (enum memState)state;
        bufRegion.bootRegionID = bootMemRegionsCount;
        bufRegion.regionID = regionsCount;
        bufRegion.next = currMemListPointer + 1; // the next Memory region would be here.

        memcpy(currMemListPointer, &bufRegion, sizeof(MemoryRegion));

        currMemListPointer++;
        regionsCount++;
    }

    // a left over memory here means the memory can't be contigious
    // this is because a state change from usable to something else implies
    // the next memory region is not usable. the only usable memory is the one
    // marked explicitly as usable.
    if (size % PHYSICAL_PAGE_SIZE != 0){
        bufRegion.baseAddress = (uint8_t *)(regionBaseAddr + i * PHYSICAL_PAGE_SIZE);
        bufRegion.size = PHYSICAL_PAGE_SIZE;
        bufRegion.state = (enum memState)state;
        bufRegion.bootRegionID = bootMemRegionsCount;
        bufRegion.regionID = regionsCount++;
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
        ptr->state = reserved;
        ptr->allocRequestID = -2;
        actualReserved += ptr->GetSize();
        sizeToReserver -= ptr->GetSize();
        ptr = ptr->next;
    }
    screen.WriteString("reserved ");
    screen.WriteInt(BYTE_TO_MB(actualReserved));
    screen.WriteString(" MBs for kernel...\n");
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
    screen.WriteString("Initializing memory...\n");
    uint64_t bootMemRegionsCount = 0;
    do{
        uint64_t size = processMemoryTableEntry(bootMemRegionsCount++);

        // This is not a reliable exit condition.
        if (size == 0){
            break;
        }

    } while(1);
    memoryListHead[regionsCount - 1].next = NULL;

    reserverKernelMemory();
    setupFreePagePtr();
    nextAllocID = 1;
}

void Memory::PrintMemory(){
    uint64_t memSizeBytes = 0;
    uint64_t availableMemory = 0;
    uint64_t reservedMemory = 0;
    for (MemoryRegion *ptr = memoryListHead; ptr != NULL; ptr = ptr->next){
        memSizeBytes += ptr->GetSize();
        if (ptr->state == usable) {
            availableMemory += ptr->GetSize();
        } else {
            reservedMemory += ptr->GetSize();
        }
    }

    screen.WriteString("System ram size is ");
    screen.WriteInt(BYTE_TO_MB(memSizeBytes));
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

        ptr->state = reserved;
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
        if (start->state != reserved){
            panic("trying to free an unreserved page");
        }

        start->state = usable;
        start->allocRequestID = 0;
        start = start->next;
    }
}

// Global Memory Variable;
Memory sysMemory;
