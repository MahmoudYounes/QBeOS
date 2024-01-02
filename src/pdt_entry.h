#ifndef PDT_ENTRY_H
#define PDT_ENTRY_H

#include "common.h"
#include "mem_encodeable.h"
#include "pt_entry.h"
#include "screen.h"
#include "formater.h"

#define KERN_PDT PDTEntry(KB).SetPresent()->SetIsReadWrite()
#define USER_PDT PDTEntry(KB).SetPresent()->SetIsReadWrite()->SetIsUserAccessible()

extern Screen screen;

enum PageSize{
    KB,
    MB,
};

class PDTEntry : public MemoryEncodeable {
    private:
        uint32_t entry;
        bool isMBPage();
    public:
        PDTEntry(PageSize ps);
        PDTEntry *SetPresent();
        PDTEntry *SetGlobal();
        PDTEntry *SetAccessed();
        PDTEntry *SetPageAttrTable();
        PDTEntry *SetDirty();
        PDTEntry *SetDisableCache();
        PDTEntry *SetPageWriteThrough();
        PDTEntry *SetIsUserAccessible();
        PDTEntry *SetIsReadWrite();
        PDTEntry *SetPTAddress(uint64_t addr);

        uint64_t EncodeEntryAt(uintptr_t addr);

        void PrintEntryInfo(char *buf);
};

#endif /* PDT_ENTRY_H */
