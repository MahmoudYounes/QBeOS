#include "include/acpi.h"

ACPIM::ACPIM() {
  bool found = false;
  uint16_t ebdaPtr;

  kprint("locating ACPI rsdt pointer\n\0");

  memcpy(&ebdaPtr, (uint8_t *)ACPI_EBDA_REG_START, 2);

  kprintf("found EBDA at %x\n\0", (uint32_t)ebdaPtr);

  char sig[9];
  int steps = 0;
  for (uintptr_t p = ebdaPtr; !found && steps < 1024; steps++) {
    memcpy(sig, (uint8_t *)p, 8);
    if (strcmp(sig, "RSD PTR ") == 0) {
      kprint("found rsdp from EBDA\n\0");
      found = true;
      parseRSDP(p);
    }
    p += 16;
  }

  for (uint8_t *p = (uint8_t *)0xe0000; !found && p < (uint8_t *)0xfffff; p += 16) {
    memcpy(sig, p, 8);
    sig[8] = '\0';
    if (strcmp(sig, "RSD PTR \0") == 0) {
      found = true;
      kprint("found rsdp from BIOS AREA\n\0");
      parseRSDP((uintptr_t)p);
    }
  }


  if (!found) {
    panic("didn't find ACPI tables\n\0");
  }
}

void ACPIM::parseRSDP(uintptr_t p) {
  memcpy(&rsdp, (uint8_t *)p, sizeof(ACPIRSDP));
  if (rsdp.rev == 0) {
    kprintf("found v1 acpi rsdp at %x\n\0", (uint32_t)p);
    parseRSDPV1();
  } else {
    kprintf("found v2 acpi rsdp at %x\n\0", (uint32_t)p);
    parseRSDPV2();
  }
}

void ACPIM::parseRSDPV1() {
#if DEBUG
  printTableInfo();
#endif
  rsdt = new RSDTM(rsdp.rsdtAddr);
}

void ACPIM::parseRSDPV2() {
#if DEBUG
  printTableInfo();
#endif
  panic("XSDT is not implemented yet\0\n");
}

void ACPIM::printTableInfo() {
  kprint("print rsdp info start:\n\0");
  char *buf = new char[256];
  memcpy(buf, rsdp.signature, 8);
  buf[8] = '\n';
  buf[9] = '\0';
  kprint(buf);

  kprintf("%d\n\0", (uint32_t)rsdp.checksum);

  memcpy(buf, rsdp.oemid, 6);
  buf[5] = '\n';
  buf[6] = '\0';
  kprint(buf);

  kprintf("%d\n\0", rsdp.rev);

  kprintf("%x\n\0", rsdp.rsdtAddr);

  kprintf("%d\n\0", rsdp.lengthBytes); 

  kprintf("%d\n\0", (uint32_t)rsdp.extChecksum);

  kprint("print rsdp info end\n\0");
}

bool ACPIM::IsPS2Supported(){
  return rsdt->IsPS2Supported();
}

