#include "acpi/include/acpi_utils.h"

uint8_t calculateChecksum(uintptr_t begin, uint32_t length) {
  uint8_t csum = 0;
  uint32_t itr = 0;
  for (uint8_t *ptr = (uint8_t *)begin; itr < length; itr++, ptr++) {
    csum += *ptr;
  }
  return csum;
}


