#ifndef MCFG_H
#define MCFG_H

#include "include/common.h"

struct MCFGHDR {
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

struct MCFGNODE{
  uint64_t baseAddr;
  uint16_t segGroup;
  uint8_t  startDec;
  uint8_t  endDec;
  uint32_t res;
};

class MCFGM {
private:
  MCFGNODE *pciNodes;

public:
  MCFGM(uintptr_t);
};

#endif
