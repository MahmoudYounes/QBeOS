#ifndef GDT_H
#define GDT_H

#include "common.h"
#include "logger.h"


#define GDT_ENTRY_SIZE_BYTES 8
#define GDT_KERNEL_CODE_DESCRIPTOR_SEL 0x8

// Access Bits Encoded

#define SEGMENT_PRESENT 1 << 7

#define SEGMENT_PRIVLG_0 0 << 5
#define SEGMENT_PRIVLG_1 1 << 5
#define SEGMENT_PRIVLG_2 2 << 5
#define SEGMENT_PRIVLG_3 3 << 5

#define SEGMENT_CODE_DATA 1 << 4

#define SEGMENT_CODE_SEGMENT 1 << 3

#define SEGMENT_NOT_CONFORMING 1 << 2

#define SEGMENT_RW_SEGMENT 1 << 1

#define SEGMENT_ACCESSED 1

#define SEGMENT_SYSTEM_TYPE_16TSS_AVAILABLE 1
#define SEGMENT_SYSTEM_TYPE_16TSS_BUSY 3
#define SEGMENT_SYSTEM_TYPE_3264TSS_AVAILABLE 9
#define SEGMENT_SYSTEM_TYPE_3264TSS_BUSY 0xb

#define SEGMENT_SYSTEM_TYPE_LDT 2

static const uint8_t SEGMENT_KERNEL_CODE = SEGMENT_PRESENT | SEGMENT_PRIVLG_0 | SEGMENT_CODE_DATA | SEGMENT_CODE_SEGMENT | SEGMENT_RW_SEGMENT;
static const uint8_t SEGMENT_KERNEL_DATA = SEGMENT_PRESENT | SEGMENT_PRIVLG_0 | SEGMENT_CODE_DATA | SEGMENT_RW_SEGMENT;
static const uint8_t SEGMENT_USER_CODE = SEGMENT_PRESENT | SEGMENT_PRIVLG_3 | SEGMENT_CODE_DATA | SEGMENT_CODE_SEGMENT | SEGMENT_RW_SEGMENT;
static const uint8_t SEGMENT_USER_DATA = SEGMENT_PRESENT | SEGMENT_PRIVLG_3 | SEGMENT_CODE_DATA |  SEGMENT_RW_SEGMENT;

// Flag Bits Encoded
#define SEGMENT_FLAGS_GRANULARITY_PAGE 1 << 3
#define SEGMENT_FLAGS_SIZE_32PROTECTED 1 << 2
#define SEGMENT_FLAGS_MODE_LONG_MODE 1 << 1

static const uint8_t SEGMENT_FLAGS_DEFAULT = SEGMENT_FLAGS_GRANULARITY_PAGE | SEGMENT_FLAGS_SIZE_32PROTECTED;

struct GDTEntry {
    uint32_t limit;
    uint32_t base;
    uint8_t access;
    uint8_t flags;
};

// GDT Manager
class GDT {
    private:
        static const uint32_t gdtBaseAddress = 0x00400000;
        uint8_t *lastEntryAddress;
        uint8_t countEntries = 0;

        void encodeEntry(GDTEntry *entry);

        int tmp = 0;
    public:
        GDT();

        GDTEntry ConstructGDTEntry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
        GDTEntry ConstructLDTEntry(uint32_t base, uint32_t limit);
        GDTEntry ConstructTSSKernEntry(uint32_t base, uint32_t limit);
        GDTEntry ConstructTSSUserEntry(uint32_t base, uint32_t limit);

        // TODO: how are you going to think about error propagation?
        void AddGDTEntry(GDTEntry *entry);

        void RefreshGDT();
};


#endif /* GDT_H */
