#include "acpi/include/rsdt.h"
#include "acpi/include/acpi.h"

RSDTM::RSDTM(uintptr_t rsdtAddr) {
  rsdtPtr = rsdtAddr;
  memcpy(&rsdt, (uint8_t *)rsdtAddr, sizeof(ACPIHDR));
  validateTable();

  if (!valid) {
    kprint("RSDT table is invalid\n\0");
    return;
  }

  char *buf = new char[256];
  kprintf(buf, "found a valid RSDT with %d entries. allocating memory...\n\0",
          rsdt.length);
  entries = new uint32_t[rsdt.length];
  kprint("allocated memory \n\0");

  uint32_t idx = 0;
  uintptr_t entriesBegin = rsdtAddr + sizeof(ACPIHDR);
  while (idx < rsdt.length) {
    entries[idx] = *((uint32_t *)(entriesBegin + idx * sizeof(uint32_t)));
    idx++;
  }

  parseTables();
}

void RSDTM::validateTable() {
  valid = true;
  char *sig = new char[5];
  memcpy(sig, &rsdt.signature, 4);
  sig[4] = '\0';

  if (strcmp(sig, "RSDT\0") != 0) {
    valid = false;
  }

  uint64_t csum = calculateChecksum(rsdtPtr, rsdt.length) % 0x100;  
  if (csum) {
    panic("read invalid RSDT Table\n\0");
  }
}

void RSDTM::parseTables() {
    for (uint32_t idx = 0; idx < rsdt.length; idx++){
        uintptr_t addr = entries[idx];
        if (strncmp((const char *)addr, "FACP", 4) == 0){
            kprint("found FACP\n");
            facpm = new FACPM(addr); 
        } else if (strncmp((const char *)addr, "APIC", 4) == 0){
            kprint("found MADT\n");
            madtm = new MADTM(addr); 
        } else if (strncmp((const char *)addr, "MCFG", 4) == 0){
            kprint("found MCFG\n");
            mcfgm = new MCFGM(addr);
        }

    } 
}
