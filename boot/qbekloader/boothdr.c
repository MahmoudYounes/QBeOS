#include "include/boothdr.h"

struct boothdr {
  uint32_t   memregions_count;
  uintptr_t  memtable;
  uint8_t    pci_supported;
  uint8_t    pci_conf_mech;
  uint8_t    pci_magic;
} boothdr;

void parse_boot_hdr(){
  uint8_t *boothdr_begin = (uint8_t*) BOOTHDR;
  
  boothdr.memregions_count = *(uint32_t*)boothdr_begin;
  boothdr_begin += sizeof(uint32_t);
  
  boothdr.memtable = *(uintptr_t*)boothdr_begin;
  boothdr_begin += sizeof(uintptr_t);
  
  boothdr.pci_supported = *boothdr_begin;
  boothdr_begin++;

  boothdr.pci_conf_mech = *boothdr_begin;
  boothdr_begin++;

  boothdr.pci_magic = *boothdr_begin;
  boothdr_begin++; 
}

bool is_pcie_supported(){
  return boothdr.pci_supported && boothdr.pci_conf_mech == 1;
}

uintptr_t get_memtable_addr(){
  return boothdr.memtable;
}

uint32_t get_memregion_count(){
  return boothdr.memregions_count;
}
