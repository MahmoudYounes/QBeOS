#ifndef GDT_H
#define GDT_H

#include "common.h"
#include "logger.h"
#include "screen.h"


// Access Bits Encoded

#define SEGMENT_PRESENT 1 << 7
#define SEGMENT_NOT_PRESENT ~SEGMENT_PRESENT

#define SEGMENT_PRIVLG_0 0 << 5
#define SEGMENT_PRIVLG_1 1 << 5
#define SEGMENT_PRIVLG_2 2 << 5
#define SEGMENT_PRIVLG_3 3 << 5

#define SEGMENT_CODE_DATA 1 << 4
#define SEGMENT_SYSTEM ~SEGMENT_CODE_DATA

#define SEGMENT_CODE_SEGMENT 1 << 3
#define SEGMENT_DATA_SEGMENT ~SEGMENT_CODE_SEGMENT

#define SEGMENT_NOT_CONFORMING 1 << 2
#define SEGMENT_CONFORMING ~SEGMENT_NOT_CONFORMING

#define SEGMENT_RW_SEGMENT 1 << 1
#define SEGMENT_NOT_RW_SEGMENT ~SEGMENT_RW_SEGMENT

#define SEGMENT_ACCESSED 1
#define SEGMENT_NOT_ACCESSED ~SEGMENT_ACCESSED

static const unsigned short SEGMENT_KERNEL_CODE = SEGMENT_PRESENT | SEGMENT_PRIVLG_0 | SEGMENT_CODE_DATA | SEGMENT_CODE_SEGMENT | SEGMENT_RW_SEGMENT;
static const unsigned short SEGMENT_KERNEL_DATA = SEGMENT_PRESENT | SEGMENT_PRIVLG_0 | SEGMENT_CODE_DATA | SEGMENT_RW_SEGMENT;
static const unsigned short SEGMENT_USER_CODE = SEGMENT_PRESENT | SEGMENT_PRIVLG_3 | SEGMENT_CODE_DATA | SEGMENT_CODE_SEGMENT | SEGMENT_RW_SEGMENT;
static const unsigned short SEGMENT_USER_DATA = SEGMENT_PRESENT | SEGMENT_PRIVLG_3 | SEGMENT_CODE_DATA |  SEGMENT_RW_SEGMENT;

// Flag Bits Encoded
#define SEGMENT_FLAGS_GRANULARITY_PAGE 1 << 3
#define SEGMENT_FLAGS_SIZE_32PROTECTED 1 << 2
#define SEGMENT_FLAGS_MODE_LONG_MODE 1 << 1

static const unsigned short SEGMENT_FLAGS_DEFAULT = SEGMENT_FLAGS_GRANULARITY_PAGE | SEGMENT_FLAGS_SIZE_32PROTECTED;

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

        // TODO: how are you going to think about error propagation?
        void AddGDTEntry(GDTEntry *entry);

        void RefreshGDT();
};


#endif /* GDT_H */
