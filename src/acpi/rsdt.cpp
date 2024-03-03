#include "acpi/include/rsdt.h"

RSDT::RSDT(uintptr_t rsdtAddr) {
  memcpy(&rsdt, (uint8_t *)rsdtAddr, sizeof(ACPIRSDT));
  validateTable();

  if (!valid) {
    kprint("RSDT table is invalid\n\0");
    return;
  }

  char *buf = new char[256];
  kprintf(buf, "found a valid RSDT with %d entries. allocating memory...\n\0",
          rsdt.length);
  entries = new uint32_t[rsdt.length];

  uint32_t idx = 0;
  uintptr_t entriesBegin = rsdtAddr + sizeof(ACPIRSDT);
  while (idx < rsdt.length) {
    entries[idx] = *((uint32_t *)(entriesBegin + idx * sizeof(uint32_t)));
    idx++;
  }
}

void RSDT::validateTable() {
  valid = true;
  char *sig = new char[5];
  memcpy(sig, &rsdt.signature, 4);
  sig[4] = '\0';

  if (strcmp(sig, "RSDT\0") != 0) {
    valid = false;
  }

  // TODO: validate checksum
}
