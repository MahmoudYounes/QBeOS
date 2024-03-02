#ifndef VMM_H
#define VMM_H

#include "common.h"
#include "mem_region.h"
#include "memory.h"
#include "pdt_entry.h"
#include "pt_entry.h"

#define ENTRIES_COUNT 1024
#define ENTRY_SIZE_BYTES sizeof(uint32_t)
#define TABLE_SIZE_BYTES ENTRIES_COUNT *ENTRY_SIZE_BYTES

// flags for mapping
#define VMM_KERN 1  // present read only
#define VMM_UNMAP 0 // same effect as RESV different naming

#define IS_ON_PAGE_BOUNDARY(addr) (addr) % PAGE_SIZE_BYTES == 0

extern Memory sysMemory;

class VirtualMemory {
  // PDT address, check OSMap.txt
  // When moving to 64 bits PDTs will exist.
  static const uintptr_t PDTAddress = 0x500000;
  static const uintptr_t PTAddress = 0x510000;
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

  // Given a virtual address return the page aligned physical address
  uintptr_t virtualToPhysicalAddr(uintptr_t vaddr);

  // Enables paging
  void enablePaging();

  // Flushes the TLB
  void flushTLB();

  // Maps a virtual address to a physical address
  void map(uintptr_t vaddr, uintptr_t paddr, uint8_t flags);

  // Unmaps a virtual address
  void unmap(uintptr_t vaddr);

public:
  VirtualMemory();
  VirtualMemory(bool shouldTestMemory);

  // Allocates virtual memory. TODO: convert to lazy allocation
  void *Allocate(size_t size);

  // Given a pointer to memory free memory
  void Free(void *ptr);

  // Given a virtual address return the Page Table Entry content
  uint32_t GetPageEntry(uintptr_t addr);

  // Maps the memory page
  void MMap(uintptr_t srcPtr, uintptr_t dstPtr);

  // Unmap addr
  void Unmap(uintptr_t dstPtr);
};

#endif /* VMM_H */
