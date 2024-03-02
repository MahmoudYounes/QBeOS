#include "acpi.h"

ACPI::ACPI() {
  char *buf = (char *)vmm.Allocate(256);

  kprint("locating ACPI rsdt pointer\n\0");
  bool found = locateRSDP(ACPI_REG3_STARTADDR, ACPI_REG3_ENDADDR);
  found = found ? found : locateRSDP(ACPI_REG2_STARTADDR, ACPI_REG2_ENDADDR);
  found = found ? found : locateRSDP(ACPI_REG3_STARTADDR, ACPI_REG3_ENDADDR);

  if (!found) {
    panic("could not find a valid ACPI table\n\0");
  }

  kprint("located ACPI rsdp\n\0");
  acpiver = acpirsdp.rev;
  kprint("parsing ACPI tables\n\0");

  vmm.Free(buf);
}

bool ACPI::locateRSDP(uintptr_t startAddr, uintptr_t endAddr) {
  char signature[8];

  for (uint64_t *p = (uint64_t *)startAddr; p < (uint64_t *)endAddr; p += 1) {
    // check signature
    memcpy(signature, p, 8);
    if (strcmp(signature, "RSD PTR ") == 0) {
      memcpy(&acpirsdp, p, sizeof(ACPIRSDP));
      return true;
    }
  }
  return false;
}

ACPI acpi;
