#ifndef ACPI_H
#define ACPI_H

#include "acpi/include/rsdt.h"
#include "acpi/include/xsdt.h"
#include "common.h"
#include "kstdlib.h"
#include "logger.h"
#include "strings.h"
#include "vmm.h"

#define ACPI_EBDA_REG_START 0x40e /* Physical Address */
#define ACPI_EBDA_REG_END 0x80e
#define ACPI_HI_RSDP_REG_START 0xe0000 /* Physical Address */
#define ACPI_HI_RSDP_REG_END 0x100000

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
  struct ACPIRSDP rsdp;
  RSDT *rsdt;
  XSDT *xsdt;

  void parseRSDP(uintptr_t p);
  void parseRSDPV1();
  void parseRSDPV2();
  void printTableInfo();

public:
  ACPI();
};

#endif /* ACPI_H */
