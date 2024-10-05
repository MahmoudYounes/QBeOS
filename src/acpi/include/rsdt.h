#ifndef RSDT_H
#define RSDT_H

#include "include/common.h"
#include "include/logger.h"
#include "include/strings.h"
#include "kstdlib/include/global_operators.h"
#include "acpi/include/facp.h"
#include "acpi/include/madt.h"
#include "acpi/include/mcfg.h"

struct ACPIHDR {
  char signature[4];
  uint32_t length;
  uint8_t revision;
  uint8_t checksum;
  char oemid[6];
  uint64_t oemTableID;
  uint32_t oemRevision;
  uint32_t creatorID;
  uint32_t creatorRevision;
} __attribute__((packed));

class RSDTM {
private:
  uintptr_t rsdtPtr;
  struct ACPIHDR rsdt;
  bool valid;
  uint32_t *entries;

  /* list of all ACPI tables suppoerted */
  FACPM *facpm;
  MADTM *madtm;
  MCFGM *mcfgm;

  void validateTable();
  void populateACPI();
  void parseTables();

public:
  RSDTM();
  RSDTM(uintptr_t rsdtAddr);

  bool IsPS2Supported();
};

#endif /* RSDT_H */
