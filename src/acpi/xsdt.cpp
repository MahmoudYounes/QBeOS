#include "acpi/include/xsdt.h"

XSDT::XSDT(uint64_t xsdtAddr) {
  memcpy(&xsdt, (uint8_t *)xsdtAddr, sizeof(ACPIXSDT));
  validateTable();

  if (!valid) {
    kprint("XSDT table is invalid\n\0");
    return;
  }

  char *buf = new char[256];
  kprintf(buf, "found a valid XSDT with %d entries. allocating memory...\n\0",
          xsdt.length);
}

void XSDT::validateTable() {
  valid = true;
  char *sig = new char[5];
  memcpy(sig, &xsdt.signature, 4);
  sig[4] = '\0';

  if (strcmp(sig, "XSDT\0") != 0) {
    valid = false;
  }

  // TODO: validate checksum
}
