#include "acpi/include/madt.h"

MADTM::MADTM(){}
MADTM::MADTM(uintptr_t addr){
    uint8_t *tableEnd;
    uint8_t *ptr;

    memcpy(&madtHeader, (void *)addr, sizeof(MADT));
    // TODO: Add table validation
    
    
    tableEnd = (uint8_t *)(addr + madtHeader.length);
    ptr = (uint8_t *) (addr + sizeof(MADT));

    while (ptr < tableEnd){
        uint8_t entryType = *ptr;
        uint8_t entryLength = *(ptr + 1);
        switch(entryType) {
            case 0: {
                LAPIC *lapicEntry = new LAPIC();
                memcpy(lapicEntry, (ptr + 2), sizeof(LAPIC));
                // TODO: Add to list 
                break;
            }
            case 1: {
                IOAPIC *ioapicEntry = new IOAPIC();
                memcpy(ioapicEntry, (ptr + 2), sizeof(IOAPIC));
                // TODO: Add to list 
                break;
            }
            default:
                break;
        }
        ptr += entryLength + 2; 
    }
    

}
