#ifndef PCIE_H
#define PCIE_H

#include "include/common.h"

#define CONFIG_ADDR 0xcf8
#define CONFIG_DATA 0xcfc

#define MAX_DEV_COUNT 256

struct device_t{
  uint8_t busnr;
  uint8_t devnr;
  uint8_t funcnr;
  uint16_t vendorid;
  uint16_t deviceid;
  uint8_t classid;
  uint8_t subclass;
  uint8_t headertype;
  union {
    // generic
    struct {
      uint32_t gen_bar0addr;
      uint32_t gen_bar1addr;
      uint32_t bar2addr;
      uint32_t bar3addr;
      uint32_t bar4addr;
      uint32_t bar5addr;
      uint32_t cisptr;
      uint16_t subsys_vendor;
      uint16_t subsys_id;
      uint32_t exp_romaddr;
      uint32_t g_res_capptr;
      uint32_t res;
      uint8_t g_intrline;
      uint8_t g_intrpin;
      uint8_t mingrant;
      uint8_t maxlat;
    };
    // bridge
    struct {
      uint32_t b_bar0addr;
      uint32_t b_bar1addr;
      uint8_t primary_busnr;
      uint8_t secondary_busnr;
      uint8_t subord_busnr;
      uint8_t secondary_lat;
      uint8_t iobase;
      uint8_t iolimit;
      uint16_t secondary_status;
      uint16_t memorybase;
      uint16_t memorylimit;
      uint16_t prefetch_membase;
      uint16_t prefetch_memlimit;
      uint32_t prefetch_baseupper;
      uint32_t prefetch_baselower;
      uint16_t iobase_upper;
      uint16_t iolimit_upper;
      uint32_t b_res_capptr;
      uint32_t exp_rombase;
      uint8_t b_intrline;
      uint8_t b_intrpin;
      uint16_t bridge_ctrl; 
    };
    // TODO: uncomment
    //struct pci_card{};
  };
};

void init_pci();
void add_device(uint8_t busnr, uint8_t devnr, uint8_t funcnr, uint32_t val);
#endif
