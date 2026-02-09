#ifndef MEMORY_H
#define MEMORY_H

#include "stddef.h"
#include "stdint.h"

#define MAX_MEM_REGIONS 256

#define INVALID_MEMT 0
#define RAM_MEMT 1
#define RES_MEMT 2
#define ACPI_MEMT 3
#define NVS_MEMT 4
#define BAD_MEMT 5
#define DISABLE_MEMT 6
#define PERSIS_MEMT 7 
#define OEM_MEMT 12

#define PAGE_SIZE 4096

struct memregion_descr{
  uint64_t startaddr;
  uint64_t sizeb;
  uint32_t rtype;
};

void init_memory();

void *malloc(size_t size);

#endif
