#include "arch/x86/include/pdt_entry.h"

PDTEntry::PDTEntry(){
  entry = 0;
}

// if PS is MB then the entry points to the physical address of a 4MB page.
// if PS is KB then the entry points to a PT where each entry points to a
// 4 KB page.
PDTEntry::PDTEntry(PageSize ps) {
  // if ps is MB then we are setting bit 7
  entry = 0 | ps << 7;
}

bool PDTEntry::isMBPage() {
  // if ps is set then this page references 4 MB page
  return entry & (1 << 7);
}

// If set the page is present in the physical memory
PDTEntry *PDTEntry::SetPresent() {
  this->entry |= (1 << 0); // consistency
  return this;
}

PDTEntry *PDTEntry::SetIsReadWrite() {
  this->entry |= (1 << 1);
  return this;
}

PDTEntry *PDTEntry::SetIsUserAccessible() {
  this->entry |= (1 << 2);
  return this;
}

PDTEntry *PDTEntry::SetPageWriteThrough() {
  this->entry |= (1 << 3);
  return this;
}

PDTEntry *PDTEntry::SetDisableCache() {
  this->entry |= (1 << 4);
  return this;
}

PDTEntry *PDTEntry::SetAccessed() {
  this->entry |= (1 << 5);
  return this;
}

PDTEntry *PDTEntry::SetDirty() {
  if (isMBPage()) {
    this->entry |= (1 << 6);
  }
  return this;
}

// If set, the processor will not invalidate TLB entry corresponding
// to page
PDTEntry *PDTEntry::SetGlobal() {
  if (isMBPage()) {
    this->entry |= (1 << 8);
  }
  return this;
}

PDTEntry *PDTEntry::SetPageAttrTable() {
  if (isMBPage()) {
    this->entry |= (1 << 12);
  }
  return this;
}

PDTEntry *PDTEntry::SetPTAddress(uint64_t addr) {
  if (isMBPage()) {
    uint64_t lowAddr = (addr >> 22) & 0x3ff; // bit 22 -> 31 = 10 bits
    uint64_t highAddr = (addr >> 32) & 0xff; // bits 32 -> 39 = 8 bits
    entry |= highAddr << 13; // bit 13 to 20 contains 32 -> 39 (highAddr)
    entry |= lowAddr << 22;  // bits 22 to 31 contains 22 -> 31 (lowAddr)
  } else {
    // taking 20 bits of addr
    uint64_t addrMasked = addr >> 12; // address is 20 MSBs
    entry |= (addrMasked << 12);
  }
  return this;
}

uint64_t PDTEntry::EncodeEntryAt(uintptr_t addr) {
  *(uint32_t *)addr = entry;
  return sizeof(entry);
}

void PDTEntry::PrintEntryInfo(char *buf) {
  kprintf(buf, "entry info %b\n\0", entry);
}
