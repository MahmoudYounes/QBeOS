#include "mem_region.h"
#include "common.h"

long MemoryRegion::GetSize(){
    return size;
}

void MemoryRegion::PrintRegionInfo(){
    screen.WriteString("Memory region ");
    screen.WriteIntToScreen(bootRegionID);
    screen.WriteString("\n  memory starts at: ");
    screen.WriteIntToScreen((uint64_t)baseAddress);
    screen.WriteString("\n  region size: ");
    if(size <= MAX_B_SIZE()){
        screen.WriteIntToScreen(size);
        screen.WriteString("bytes");
    } else if (size <= MAX_KB_SIZE()) {
        screen.WriteIntToScreen(BYTE_TO_KB(size));
        screen.WriteString("Kbs");
    } else if (size <= MAX_MB_SIZE()){
        screen.WriteIntToScreen(BYTE_TO_MB(size));
        screen.WriteString("Mbs");
    } else {
        screen.WriteIntToScreen(BYTE_TO_GB(size));
        screen.WriteString("Gbs");
    }

    screen.WriteString("\n  region type: ");
    switch (state) {
        case 0:
            screen.WriteString("invalid");
            break;
        case 1:
            screen.WriteString("usable");
            break;
        case 2:
            screen.WriteString("reserved");
            break;
        case 3:
            screen.WriteString("reclaimable");
            break;
        case 4:
            screen.WriteString("nvs");
            break;
        case 5:
            screen.WriteString("badmem");
            break;
        default:
            screen.WriteString("unknown memory type");
            break;
    }
    screen.WriteString("\n");
}
