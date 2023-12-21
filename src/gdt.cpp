#include "gdt.h"

extern Screen screen;

GDT::GDT(){
    screen.WriteString("initializing GDT...\n");

    lastEntryAddress = (uint8_t *)gdtBaseAddress;
    GDTEntry nullDescriptor = ConstructGDTEntry(0x00000000, 0x00000000, 0x0000, 0x0000);
    GDTEntry kernelCodeDescriptor = ConstructGDTEntry(0x00000000, 0xfffff, SEGMENT_KERNEL_CODE, SEGMENT_FLAGS_DEFAULT);
    GDTEntry kernelDataDescriptor = ConstructGDTEntry(0x00000000, 0xfffff, SEGMENT_KERNEL_DATA, SEGMENT_FLAGS_DEFAULT);
    GDTEntry restCodeDescriptor = ConstructGDTEntry(0x00c00000, 0xfffff, SEGMENT_USER_CODE, SEGMENT_FLAGS_DEFAULT);
    GDTEntry restDataDescriptor = ConstructGDTEntry(0x00c00000, 0xfffff, SEGMENT_USER_DATA, SEGMENT_FLAGS_DEFAULT);

    encodeEntry(&nullDescriptor);
    encodeEntry(&kernelCodeDescriptor);
    encodeEntry(&kernelDataDescriptor);
    encodeEntry(&restCodeDescriptor);
    encodeEntry(&restDataDescriptor);

    RefreshGDT();
}

void GDT::encodeEntry(GDTEntry *entry) {
    // Limit
    lastEntryAddress[0] = GET_BYTE(entry->limit, 0);
    lastEntryAddress[1] = GET_BYTE(entry->limit, 1);
    lastEntryAddress[6] = GET_BYTE(entry->limit, 2) & 0x0f;

    // Base
    lastEntryAddress[2] = GET_BYTE(entry->base, 0);
    lastEntryAddress[3] = GET_BYTE(entry->base, 1);
    lastEntryAddress[4] = GET_BYTE(entry->base, 2);
    lastEntryAddress[7] = GET_BYTE(entry->base, 3);

    // Access
    lastEntryAddress[5] = entry->access;

    // Flags
    lastEntryAddress[6] = (entry->flags << 4) | lastEntryAddress[6];

    lastEntryAddress += 8;
    countEntries++;
}

GDTEntry GDT::ConstructGDTEntry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags){
    return GDTEntry{
    limit=limit,
    base = base,
    access = access,
    flags = flags
    };
}

void GDT::RefreshGDT(){
    asm volatile(
        "cli\n\t"
        "pushad\n\t"
        "mov [esp-6], %0\n\t"
        "mov [esp-4], %1\n\t"
        "lgdt [esp-6]\n\t"
        "mov eax, 0x10\n\t"
        "mov ds, eax\n\t"
        "mov es, eax\n\t"
        "mov ss, eax\n\t"
        "mov fs, eax\n\t"
        "mov gs, eax\n\t"
        "jmp 0x8:.reload_gdt\n\t"
        ".reload_gdt:\n\t"
        "popad\n\t"
        :
        : "a"(8 * 8 * countEntries), "r"(gdtBaseAddress)
        : "memory");
}

GDT gdt;
