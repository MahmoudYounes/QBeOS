#include "arch/include/vmm.h"
#include "arch/x86/include/pt_entry.h"

static char *buf;

void VirtualMemory::testVirtualMemory() {
  if (!shouldTestMemoryBeforePaging) {
    return;
  }

  for (uintptr_t i = 0x0; i < 0x1000000; i += 4) {
    MemoryRegion mem = sysMemory.GetPageAt(i) ;
    if (mem.state != usable){
      continue;
    }

    testAddrTranslation(i);
  }
}

void VirtualMemory::testAddrTranslation(uintptr_t expectedAddr) {
  // the first 10 MSB are offset into PDT. entry contains address to the PT
  uint64_t pdtOffset = expectedAddr >> 22;

  // the next 10 MSB are offset into th PT. entry contains address to the page
  uint64_t ptOffset = (expectedAddr >> 12) & 0x3ff;
  
  // the last 12 are offset into the page
  uint64_t offsetInPage = expectedAddr & 0xfff;

  uintptr_t pdtEntryPtr = PDTAddress + pdtOffset * ENTRY_SIZE_BYTES;
  uint32_t pdtEntry = *(uint32_t *)pdtEntryPtr;

  uintptr_t ptPtr = (pdtEntry >> 12) << 12;
  uintptr_t ptEntryPtr = ptPtr + ptOffset * ENTRY_SIZE_BYTES;
  uintptr_t pteContent = *(uint32_t *)ptEntryPtr;
  uintptr_t paddr = (pteContent >> 12) << 12;
  uintptr_t addr = paddr | offsetInPage;

  if (addr != expectedAddr) {
    kprintf(buf, "testing mapping of address %p\n\0", expectedAddr);
    kprintf(buf, "page tables are incorrectly set up. expected %p found %p\n\0",
            expectedAddr, addr);
    kprintf(buf, "offseting pdt %p with %x yielded \0", PDTAddress, pdtOffset);
    kprintf(buf, "%p\n\0", pdtEntryPtr);
    kprintf(buf, "addresss of PT from PDT Entry is %p\n\0", ptPtr);
    kprintf(buf, "offseting pt %p with %d yielded \0", ptPtr, ptOffset);
    kprintf(buf, "%p\n\0", ptEntryPtr);
    kprintf(buf, "address of Page from PT Entry is %p\n\0", paddr);
    kprintf(buf,
            "constructing address from Page Base %p, and offset %d in page "
            "yielded \0",
            paddr, offsetInPage);
    kprintf(buf, "%p\n\0", addr);
    panic("vmm test failed");
  }
}

void VirtualMemory::initializeMemory() {
  kprint("Initializing virtual memory module...\n\0");
  setupPageDirectoryTable();
  mapSystemMemory();
  enablePaging();
}

VirtualMemory::VirtualMemory() { initializeMemory(); }

VirtualMemory::VirtualMemory(bool shouldTestMemory) {
  shouldTestMemoryBeforePaging = shouldTestMemory;
  initializeMemory();
}

void VirtualMemory::setupPageDirectoryTable() {
  for (int i = 0; i < ENTRIES_COUNT; i++) {
    uintptr_t pdtAddr = PDTAddress + i * ENTRY_SIZE_BYTES;
    uintptr_t ptAddr = PTAddress + i * ENTRIES_COUNT * ENTRY_SIZE_BYTES;
    createPDTEntry(pdtAddr, ptAddr);
  }
}

/**
* Maps the system memory:
* - create an identity map of the memory
* - Should map the kernel but does not.
*   for now I don't have a way to know how big is the kernel
*   so I use estimate from the output of ls -lah on the image + some buffer
*   this should change if the kernel is built as ELF file. I can then parse
*   the ELF data and then get an estimate of how big the kernel really is
* - map any reserved memory and known to be reserved memory.
*/
void VirtualMemory::mapSystemMemory() {
  MemoryInfo physicalMemInfo = sysMemory.GetMemoryInfo();
  // creating identity page directory tables and page tables
  MemoryRegion *ptr = (MemoryRegion *)physicalMemInfo.pagesWalker;
  while (ptr != NULL) {
    uint32_t flags;
    if (ptr->state == usable) {
      flags = VMM_KERN;
    } else {
      flags = VMM_UNMAP; 
    }  
    map((uintptr_t)ptr->baseAddress, (uintptr_t)ptr->baseAddress, flags);
    ptr = ptr->next;
  }

  // mapping video memory. this is the only thing that gets initalized
  // before the vmm. after that, components should map their own memory
  map(0xb8000, 0xb8000, VMM_KERN);
}

void VirtualMemory::enablePaging() {
  // test translate if enabled
  testVirtualMemory();

  pagingEnabled = true;
  // TODO: if we want higher half kernel then after paging is enabled, must do a
  // far jump to the next kernl address
  __asm__ __volatile__("mov ecx, %0\n\t"
                       "mov cr3, ecx\n\t"
                       "xor ecx, ecx\n\t"
                       "mov ecx, cr0\n\t"
                       "or ecx, 0x80000001\n\t"
                       "mov cr0, ecx\n\t"
                       :
                       : "r"(PDTAddress));
  kprint("Enabled paging...\n\0");
}

void VirtualMemory::createPDTEntry(uintptr_t atPDTPtr, uintptr_t ofPTPtr) {
  // this creates a variable on the stack when it calls the function. it will be cleaned up.
  KERN_PDT->SetPTAddress(ofPTPtr)->EncodeEntryAt(atPDTPtr);
}

void VirtualMemory::createPTEntry(uintptr_t atPTPtr, uintptr_t ofPtr,
                                  uint8_t flags) {
  if (flags & VMM_KERN) {
    KERN_PT->SetPageAddress(ofPtr)->EncodeEntryAt(atPTPtr);
  } else if (flags & VMM_UNMAP) {
    NONPRESENT_PT.SetPageAddress(0x0)->EncodeEntryAt(atPTPtr);
  } else {
    // TODO: once we have IDT, lazy create these virual pages on page faults?
    USER_PT->SetPageAddress(ofPtr)->EncodeEntryAt(atPTPtr);
  }
}

void VirtualMemory::map(uintptr_t vaddr, uintptr_t paddr, uint8_t flags) {
  // offsetInPDT
  uint64_t pdtOffset = vaddr >> 22;
  
  // offsetInPT
  uint64_t ptOffset = (vaddr >> 12) & 0x3ff;
  
  // address of where the page begins
  uintptr_t pageFrameAddress = paddr & ~0xfff;
  
  uintptr_t pdtEntryAddr = PDTAddress + pdtOffset * ENTRY_SIZE_BYTES;
  uintptr_t ptAddr = (*(uint32_t *)pdtEntryAddr) & ~0xfff;
  uintptr_t ptEntryAddr = ptAddr + ptOffset * ENTRY_SIZE_BYTES;

  createPTEntry((uintptr_t)ptEntryAddr, pageFrameAddress, flags);
  
  if (pagingEnabled){
    flushTLB();
  }
}

uint32_t VirtualMemory::GetPageEntry(uintptr_t vaddr) {
  uint64_t pdtOffset = vaddr >> 22;
  uint64_t ptOffset = (vaddr >> 12) & 0x3ff;
  uintptr_t pdtAddr = PDTAddress + pdtOffset * ENTRY_SIZE_BYTES;
  uintptr_t ptAddr = ((*(uint32_t *)pdtAddr) >> 12) << 12;
  uintptr_t ptEntryAddr = ptAddr + ptOffset * ENTRY_SIZE_BYTES;
  return *(uint32_t *)ptEntryAddr;
}

// TODO: there are two ways to implement this. Either allocate the entire memory
// physically then virtually map it, or allocate page by page. when you have
// time tracers evaluate which is better.
void *VirtualMemory::Allocate(size_t size) {
  if (size == 0) {
    // hathzar hanhazar
    return NULL;
  }

  uint32_t numPages;
  if (size % PAGE_SIZE_BYTES != 0) {
    numPages = (size + PAGE_SIZE_BYTES) / PAGE_SIZE_BYTES; // math.ciel
  } else {
    numPages = size / PAGE_SIZE_BYTES;
  }

  uintptr_t allocatedPage = (uintptr_t)sysMemory.AllocPhysicalPage();
  map(allocatedPage, allocatedPage, VMM_KERN);
  for (uint32_t i = 0; i < numPages - 1; i++) {
    uintptr_t pptr = (uintptr_t)sysMemory.AllocPhysicalPage();
    map(pptr, pptr, VMM_KERN);
  }
  return (void *)allocatedPage;
}

void VirtualMemory::unmap(uintptr_t vaddr) {
  uint64_t pdtOffset = vaddr >> 22;
  uint64_t ptOffset = (vaddr >> 12) & 0x3ff;
  uintptr_t pdtAddr = PDTAddress + pdtOffset * ENTRY_SIZE_BYTES;
  uintptr_t ptAddr = ((*(uint32_t *)pdtAddr) >> 12) << 12;
  uintptr_t ptEntryAddr = ptAddr + ptOffset * ENTRY_SIZE_BYTES;

  createPTEntry((uintptr_t)ptEntryAddr, 0, VMM_UNMAP);
}

void VirtualMemory::Free(void *ptr) {
  uintptr_t paddr = virtualToPhysicalAddr((uintptr_t)ptr);
  uintptr_t vptr = (uintptr_t)ptr;
  MemoryRegion startPage = sysMemory.GetPageAt(paddr);
  uint64_t allocId = startPage.allocRequestID;

  for (MemoryRegion *walker = &startPage; walker != NULL;
       walker = walker->next) {
    unmap(vptr);
    if (walker->allocRequestID != allocId) {
      break;
    }

    vptr += PAGE_SIZE_BYTES;
    walker = walker->next;
  }
  sysMemory.Free((void *)paddr);
  flushTLB();
}

uintptr_t VirtualMemory::virtualToPhysicalAddr(uintptr_t vaddr) {
  uintptr_t paddr;
  uint32_t ent = GetPageEntry(vaddr);
  paddr = (ent >> 12) << 12;
  return paddr;
}

// TODO: Consider implementing the signle version of TLB flush
void VirtualMemory::flushTLB() {
  __asm__ __volatile("mov eax, cr3\n\t"
                     "mov cr3, eax\n\t");
}

void VirtualMemory::MMap(uintptr_t srcPtr, uintptr_t dstPtr) {
  map(dstPtr, srcPtr, VMM_KERN);
}

void VirtualMemory::Unmap(uintptr_t dstPtr) { unmap(dstPtr); }

void *VirtualMemory::getPage(){
  return sysMemory.AllocPhysicalPage();
}

