#include "include/acpi.h"

ACPI::ACPI() {
  bool found = false;
  char *buf = new char[256];
  uint16_t ebdaPtr;

  kprint("locating ACPI rsdt pointer\n\0");

  memcpy(&ebdaPtr, (uint8_t *)ACPI_EBDA_REG_START, 2);

  kprintf(buf, "found EBDA at %x\n\0", (uint32_t)ebdaPtr);

  char sig[9];
  int steps = 0;
  for (uintptr_t p = ebdaPtr; steps < 1024; steps++) {
    memcpy(sig, (uint8_t *)p, 8);
    if (strcmp(sig, "RSD PTR ") == 0) {
      kprint("found rsdp from EBDA\n\0");
      found = true;
      parseRSDP(p);
    }
    p += 16;
  }

  for (uint8_t *p = (uint8_t *)0xe0000; p < (uint8_t *)0xfffff; p += 16) {
    memcpy(sig, p, 8);
    sig[8] = '\0';
    if (strcmp(sig, "RSD PTR \0") == 0) {
      found = true;
      kprint("found rsdp from BIOS AREA\n\0");
      parseRSDP((uintptr_t)p);
    }
  }

  delete[] buf;

  if (!found) {
    panic("didn't find ACPI tables\n\0");
  }
}

void ACPI::parseRSDP(uintptr_t p) {
  char *buf = new char[256];
  memcpy(&rsdp, (uint8_t *)p, sizeof(ACPIRSDP));
  if (rsdp.rev == 0) {
    kprintf(buf, "found v1 acpi rsdp at %x\n\0", (uint32_t)p);
    parseRSDPV1();
  } else {
    kprintf(buf, "found v2 acpi rsdp at %x\n\0", (uint32_t)p);
    parseRSDPV2();
  }

  delete[] buf;
}

void ACPI::parseRSDPV1() {
  printTableInfo();
  rsdt = new RSDT(rsdp.rsdtAddr);
  HLT();
}

void ACPI::parseRSDPV2() {
  printTableInfo();
  xsdt = new XSDT(rsdp.xsdtAddr);
  HLT();
}

void ACPI::printTableInfo() {
  kprint("print rsdp info start:\n\0");
  char *buf = new char[256];
  memcpy(buf, rsdp.signature, 8);
  buf[8] = '\n';
  buf[9] = '\0';
  kprint(buf);

  kprintf(buf, "%d\n\0", (uint32_t)rsdp.checksum);

  memcpy(buf, rsdp.oemid, 6);
  buf[5] = '\n';
  buf[6] = '\0';
  kprint(buf);

  kprintf(buf, "%d\n\0", rsdp.rev);

  kprintf(buf, "%x\n\0", rsdp.rsdtAddr);

  kprintf(buf, "%d\n\0", rsdp.lengthBytes);

  kprintf(buf, "%X\n\0", rsdp.xsdtAddr);

  kprintf(buf, "%d\n\0", (uint32_t)rsdp.extChecksum);

  kprint("print rsdp info end\n\0");
}

ACPI acpi;
