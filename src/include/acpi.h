#ifndef ACPI_H
#define ACPI_H

#include "common.h"
#include "logger.h"
#include "strings.h"
#include "vmm.h"

#define ACPI_REG1_STARTADDR 0x80000
#define ACPI_REG1_ENDADDR 0xa0000

#define ACPI_REG2_STARTADDR 0xe0000
#define ACPI_REG2_ENDADDR 0x100000

#define ACPI_REG3_STARTADDR 0xbfff0000
#define ACPI_REG3_ENDADDR 0xc0000000

extern VirtualMemory vmm;

struct ACPIRSDP {
  char signature[8];
  uint8_t checksum;
  char oemid[6];
  uint8_t rev;
  uint32_t rsdtAddr;
  uint32_t lengthBytes;
  uint64_t xsdtAddr;
  uint8_t extChecksum;
  uint8_t reserved[3];
} __attribute__((packed));

class ACPI {
private:
  uint8_t acpiver;
  struct ACPIRSDP acpirsdp;

  bool locateRSDP(uintptr_t startAddr, uintptr_t endAddr);

public:
  ACPI();
};

#endif /* ACPI_H */
