#ifndef XSDT_H
#define XSDT_H

#include "include/common.h"
#include "include/logger.h"
#include "include/strings.h"
#include "kstdlib/include/global_operators.h"

struct ACPIXSDT {
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

class XSDT {
private:
  struct ACPIXSDT xsdt;
  bool valid;
  void validateTable();

public:
  XSDT(uint64_t xsdtAddr);
};

#endif /* XSDT_H */
