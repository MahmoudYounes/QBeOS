#ifndef PT_ENTRY_H
#define PT_ENTRY_H

#include "include/common.h"
#include "include/mem_encodeable.h"

#define KERN_PT PTEntry().SetPresent()->SetIsReadWrite()
#define INVALID_PT PTEntry()
#define USER_PT PTEntry().SetPresent()->SetIsUserAccessible()->SetIsReadWrite()

class PTEntry : public MemoryEncodeable {
private:
  uint32_t entry;

public:
  PTEntry();
  PTEntry *SetPresent();
  PTEntry *SetGlobal();
  PTEntry *SetAccessed();
  PTEntry *SetPageAttrTable();
  PTEntry *SetDirty();
  PTEntry *SetDisableCache();
  PTEntry *SetPageWriteThrough();
  PTEntry *SetIsUserAccessible();
  PTEntry *SetIsReadWrite();
  PTEntry *SetPageAddress(uint32_t addr);
  uint64_t EncodeEntryAt(uintptr_t addr);
};

#endif /* PT_ENTRY_H */
