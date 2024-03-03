#include "arch/x86/include/pt_entry.h"

PTEntry::PTEntry() {
  // if ps is MB then we are setting bit 7
  entry = 0;
}

// If set the page is present in the physical memory
PTEntry *PTEntry::SetPresent() {
  this->entry |= (1 << 0); // consistency
  return this;
}

PTEntry *PTEntry::SetIsReadWrite() {
  this->entry |= (1 << 1);
  return this;
}

PTEntry *PTEntry::SetIsUserAccessible() {
  this->entry |= (1 << 2);
  return this;
}

PTEntry *PTEntry::SetPageWriteThrough() {
  this->entry |= (1 << 3);
  return this;
}

PTEntry *PTEntry::SetDisableCache() {
  this->entry |= (1 << 4);
  return this;
}

PTEntry *PTEntry::SetAccessed() {
  this->entry |= (1 << 5);
  return this;
}

PTEntry *PTEntry::SetDirty() {
  this->entry |= (1 << 6);
  return this;
}

PTEntry *PTEntry::SetPageAttrTable() {
  this->entry |= (1 << 7);
  return this;
}

// If set, the processor will not invalidate TLB entry corresponding
// to page
PTEntry *PTEntry::SetGlobal() {
  this->entry |= (1 << 8);
  return this;
}

PTEntry *PTEntry::SetPageAddress(uint32_t addr) {
  // taking 20 bits of addr
  uint64_t addrMasked = addr >> 12;
  entry |= (addrMasked << 12);
  return this;
}

uint64_t PTEntry::EncodeEntryAt(uintptr_t addr) {
  *(uint32_t *)addr = entry;
  return sizeof(entry);
}
