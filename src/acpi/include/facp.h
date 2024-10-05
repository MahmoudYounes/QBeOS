#ifndef FACP_H
#define FACP_H

#include "include/common.h"
#include "include/strings.h"
#include "include/logger.h"
#include "acpi/include/acpi_utils.h"

struct GenericAddressStructure
{
  uint8_t AddressSpace;
  uint8_t BitWidth;
  uint8_t BitOffset;
  uint8_t AccessSize;
  uint64_t Address;
};

struct FACPHDR{
    uint8_t signature[4];
    uint8_t length[4];
    uint8_t mjrver;
    uint8_t checksum;
    uint8_t oemid[6];
    uint8_t oemtableid[8];
    uint8_t oemrev[4];
    uint8_t creatorid[4];
    uint8_t creatorrev[4];
    uint8_t firmwctrl[4];
    uint8_t dsdtptr[4];
    uint8_t res1;
    uint8_t preferredPowerManagementProfile;
    uint8_t sciInterrupt[2];
    uint8_t smiCommandPort[4];
    uint8_t acpiEnable;
    uint8_t acpiDisable;
    uint8_t s4BIOSREQ;
    uint8_t pstateControl;
    uint8_t pm1aEventBlock[4];
    uint8_t pm1bEventBlock[4];
    uint8_t pm1aControlBlock[4];
    uint8_t pm1bControlBlock[4];
    uint8_t pm2ControlBlock[4];
    uint8_t pmTimerBlock[4];
    uint8_t gpe0Block[4];
    uint8_t gpe1Block[4];
    uint8_t pm1EventLength;
    uint8_t pm1ControlLength;
    uint8_t pm2ControlLength;
    uint8_t pmTimerLength;
    uint8_t gpe0Length;
    uint8_t gpe1Length;
    uint8_t gpe1Base;
    uint8_t cstateControl;
    uint8_t worstC2Latency[2];
    uint8_t worstC3Latency[2];
    uint8_t flushSize[2];
    uint8_t flushStride[2];
    uint8_t dutyOffset;
    uint8_t dutyWidth;
    uint8_t dayAlarm;
    uint8_t monthAlarm;
    uint8_t century;

    // reserved in ACPI 1.0; used since ACPI 2.0+
    uint8_t bootArchitectureFlags[2];
    uint8_t reserved2;
    uint8_t flags[4];
    GenericAddressStructure ResetReg;
    uint8_t  resetValue;
    uint8_t  reserved3[3];

    // 64bit pointers - Available on ACPI 2.0+
    uint8_t                xfirmwareControl[8];
    uint8_t                xdsdt[8];
    GenericAddressStructure xpm1aEventBlock;
    GenericAddressStructure xpm1bEventBlock;
    GenericAddressStructure xpm1aControlBlock;
    GenericAddressStructure xpm1bControlBlock;
    GenericAddressStructure xpm2ControlBlock;
    GenericAddressStructure xpmTimerBlock;
    GenericAddressStructure xgpe0Block;
    GenericAddressStructure xgpe1Block;
    uint8_t sleepCtrlReg[12];
    uint8_t sleepStatusReg[12];
    uint8_t hyperVisorID[8];
};


/**
 * facp is fadt
 * You would use this extensivly when implementing SMI and PM modes
 */
class FACPM{
private:
    struct FACPHDR facphdr;
    bool valid;
    bool validateTableSignature(uintptr_t addr);
    bool validateChecksum();
public:
    FACPM();
    FACPM(uintptr_t);

    bool IsPS2Supported();
};


#endif /* FACP_H */
