#ifndef BOOTHDR_H
#define BOOTHDR_H
#include "include/common.h"

#define BOOTHDR 0x78000

void parse_boot_hdr();

bool is_pcie_supported();
uintptr_t get_memtable_addr();
uint32_t get_memregion_count();
#endif
