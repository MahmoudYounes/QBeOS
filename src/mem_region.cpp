#include "include/mem_region.h"

static char buf[512];

long MemoryRegion::GetSize() { return size; }

void MemoryRegion::PrintRegionInfo() {
  kprintf(buf, "memory region %d\n\0", bootRegionID);
  kprintf(buf, "  start address: %p\n\0", baseAddress);
  kprintf(buf, "  region size: %d Kbs\n\0", BYTE_TO_KB(size));
  kprint("  region type: \0");
  switch (state) {
  case 0:
    kprint("invalid\n\0");
    break;
  case 1:
    kprint("usable\n\0");
    break;
  case 2:
    kprint("reserved\n\0");
    break;
  case 3:
    kprint("reclaimable\n\0");
    break;
  case 4:
    kprint("nvs\n\0");
    break;
  case 5:
    kprint("badmem\n\0");
    break;
  case 6:
    kprint("kernel\n\0");
    break;
  default:
    kprint("unknown memory type\n\0");
    break;
  }
}
