#ifndef MADT_H
#define MADT_H

#include "include/common.h"
#include "include/strings.h"
#include "arch/include/vmm.h"

struct MADT{
    uint32_t signature;
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    uint8_t oemID[6];
    uint8_t oemTableId[8];
    uint32_t oemRevision;
    uint32_t creatorID;
    uint32_t creatorRevision;
    uint32_t lapicAddress;
    uint32_t lapicFlags;
};

struct LAPIC {
    uint8_t processorID;
    uint8_t lapicID;
    uint32_t flags;
};

struct IOAPIC {
    uint8_t ioapicID;
    uint8_t reserved;
    uint32_t ioapicAddr;
    uint32_t globalSystemInterruptBase;
};

struct IOAPICIntrruptSourceOverride {
    uint8_t busSource;
    uint8_t irqSource;
    uint32_t globalSystemInterrupt;
    uint16_t flags;
};

struct IOAPICNMISource {
    uint8_t nmiSource;
    uint8_t reserved;
    uint16_t flags;
    uint32_t globalSystemInterrupt;
};

struct LAPICNMI {
    uint8_t processorID;
    uint16_t flags;
    uint8_t lintNum;
};

struct LAPICAddressOverride {
    uint16_t reserved;
    uint64_t lapicAddr;
};

struct X2LAPIC {
    uint16_t reserved;
    uint32_t processorID;
    uint32_t flags;
    uint32_t acpiID;
};

class MADTM{
private:
    MADT madtHeader;
    LAPIC *lapicList; 
public:
    MADTM();
    MADTM(uintptr_t);
};

#endif

