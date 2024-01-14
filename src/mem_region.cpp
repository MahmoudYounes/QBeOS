#include "mem_region.h"

static char buf[512];

long MemoryRegion::GetSize(){
    return size;
}

void MemoryRegion::PrintRegionInfo(){
    printf(buf, "memory region %d\n\0", bootRegionID);
    printf(buf, "  start address: %p\n\0", baseAddress);
    printf(buf, "  region size: %d Kbs\n\0", BYTE_TO_KB(size));
    print("  region type: \0");
    switch (state) {
        case 0:
            print("invalid\n\0");
            break;
        case 1:
            print("usable\n\0");
            break;
        case 2:
            print("reserved\n\0");
            break;
        case 3:
            print("reclaimable\n\0");
            break;
        case 4:
            print("nvs\n\0");
            break;
        case 5:
            print("badmem\n\0");
            break;
        case 6:
            print("kernel\n\0");
            break;
        default:
            print("unknown memory type\n\0");
            break;
    }
}
