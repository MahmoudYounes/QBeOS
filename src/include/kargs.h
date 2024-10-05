#ifndef KARGS_H
#define KARGS_H

#include "include/common.h"

#define BOOT_MAGIC 0x55be

// struct used to preserve kernel arguments passed from boot loader
struct kargs{
  uint32_t memRegionsCount;
  uint32_t memTableStartAddr;
  uint8_t  pciSupported;
  uint8_t pciConfigMech;
  uint32_t magic;
};



#endif
