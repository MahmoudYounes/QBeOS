#ifndef FACP_H
#define FACP_H

#include "include/common.h"

struct FACP{
    uint32_t signature;
    uint32_t length;
    uint8_t mjrver;
    uint8_t checksum;
    uint32_t oemidhi;
    uint16_t oemidlo;
    uint32_t oemrev;
    uint32_t creatorid;
    uint32_t creatorrev;
    uint32_t firmwctrl;
    uint32_t dsdtptr;
    uint8_t res;
    uint8_t pref_pm_prof;
    uint16_t sci_int;
    uint32_t smi_cmd;
    uint8_t acpi_enable;
};

class FACPM{
private:
    struct FACP facp;
    bool valid;
public:
    FACPM();
    FACPM(uintptr_t);
};


#endif /* FACP_H */
