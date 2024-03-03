#ifndef RSDT_H
#define RSDT_H

#include "include/common.h"
#include "include/logger.h"
#include "include/strings.h"
#include "kstdlib/include/global_operators.h"

struct ACPIRSDT {
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

class RSDT {
private:
  struct ACPIRSDT rsdt;
  bool valid;
  uint32_t *entries;

  void validateTable();

public:
  RSDT(uintptr_t rsdtAddr);
};

#endif /* RSDT_H */
