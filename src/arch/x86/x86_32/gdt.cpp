#include "arch/x86/include/gdt.h"
#include "include/common.h"

GDT::GDT() {
  kprint("initializing GDT...\n");

  lastEntryAddress = (uint8_t *)gdtBaseAddress;
  GDTEntry nullDescriptor =
      ConstructGDTEntry(0x00000000, 0x00000000, 0x0000, 0x0000);
  GDTEntry kernelCodeDescriptor = ConstructGDTEntry(
      0x00000000, 0xfffff, SEGMENT_KERNEL_CODE, SEGMENT_FLAGS_DEFAULT);
  GDTEntry kernelDataDescriptor = ConstructGDTEntry(
      0x00000000, 0xfffff, SEGMENT_KERNEL_DATA, SEGMENT_FLAGS_DEFAULT);
  GDTEntry userCodeDescriptor = ConstructGDTEntry(
      0x00c00000, 0xfffff, SEGMENT_USER_CODE, SEGMENT_FLAGS_DEFAULT);
  GDTEntry userDataDescriptor = ConstructGDTEntry(
      0x00c00000, 0xfffff, SEGMENT_USER_DATA, SEGMENT_FLAGS_DEFAULT);

  AddGDTEntry(&nullDescriptor);
  AddGDTEntry(&kernelCodeDescriptor);
  AddGDTEntry(&kernelDataDescriptor);
  AddGDTEntry(&userCodeDescriptor);
  AddGDTEntry(&userDataDescriptor);

  RefreshGDT();
}

void GDT::encodeEntry(GDTEntry *entry) {
  if ((uintptr_t)lastEntryAddress - gdtBaseAddress > MB_TO_BYTE(1)) {
    panic("GDT Overflow. Cannot encode more GDT entries");
  }

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
}

GDTEntry GDT::ConstructGDTEntry(uintptr_t base, uintptr_t limit, uint8_t access,
                                uint8_t flags) {
  return GDTEntry{
      .limit = limit, .base = base, .access = access, .flags = flags};
}

GDTEntry GDT::ConstructLDTEntry(uint32_t base, uint32_t limit) {
  uint8_t access = SEGMENT_PRESENT | SEGMENT_PRIVLG_3 | SEGMENT_SYSTEM_TYPE_LDT;
  return GDTEntry{.limit = limit,
                  .base = base,
                  .access = access,
                  .flags = SEGMENT_FLAGS_DEFAULT};
}

GDTEntry GDT::ConstructTSSKernEntry(uintptr_t base, uintptr_t limit) {
  uint8_t access = SEGMENT_PRESENT | SEGMENT_PRIVLG_0 |
                   SEGMENT_SYSTEM_TYPE_3264TSS_AVAILABLE;
  uint8_t flags = SEGMENT_FLAGS_SIZE_32PROTECTED;
  return GDTEntry{
      .limit = limit, .base = base, .access = access, .flags = flags};
}

GDTEntry GDT::ConstructTSSUserEntry(uintptr_t base, uintptr_t limit) {
  uint8_t access = SEGMENT_PRESENT | SEGMENT_PRIVLG_3 |
                   SEGMENT_SYSTEM_TYPE_3264TSS_AVAILABLE;
  return GDTEntry{.limit = limit,
                  .base = base,
                  .access = access,
                  .flags = SEGMENT_FLAGS_DEFAULT};
}

void GDT::RefreshGDT() {
  uint16_t gdtSize = BITS_PER_BYTE * GDT_ENTRY_SIZE_BYTES * countEntries;
  asm volatile("cli\n\t"
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
               "jmp 0x8:1f\n\t"
               "1:\n\t"
               "popad\n\t"
               :
               : "a"(gdtSize), "r"(gdtBaseAddress)
               : "memory");
}

uint32_t GDT::AddGDTEntry(GDTEntry *entry) {
  encodeEntry(entry);
  uint32_t entrySelector = countEntries++ << 3;
  return entrySelector;
}

GDT gdt;
