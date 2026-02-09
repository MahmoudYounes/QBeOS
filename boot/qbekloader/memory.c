#include "include/memory.h"
#include "include/boothdr.h"

static uintptr_t memory_table_p;
static uint32_t memory_regions_count;
static struct memregion_descr memregions[MAX_MEM_REGIONS];

void init_memory(){
  memory_regions_count = get_memregion_count();  
  memory_table_p = get_memtable_addr();
  struct memregion_descr *descrp = (struct memregion_descr*)memory_table_p;
  for (uint32_t idx = 0; idx < memory_regions_count; idx++){
    memregions[idx].startaddr = descrp->startaddr;
    memregions[idx].sizeb = descrp->sizeb;
    memregions[idx].rtype = descrp->rtype;
    descrp++; 
  }
}

/*
 * For the sake of this simple bootloader a simple physical memory allocator
 * is used. No need to de-allocate memory here as the kernel will not make
 * any assumptions and will setup the memory as it likes.
 * To avoid any overwrites, we always allocate memory from the end of the region.
 */ 
void *malloc(size_t size){
  uint32_t pages_count = (size + PAGE_SIZE) / PAGE_SIZE;
  for (uint32_t idx = 0; idx < memory_regions_count; idx++){
    if (memregions[idx].rtype == RAM_MEMT){
      uint8_t *end_addr = (uint8_t *)(memregions[idx].startaddr + memregions[idx].sizeb);
      
      end_addr -= pages_count * PAGE_SIZE;
      memregions[idx].sizeb -= pages_count * PAGE_SIZE;
      return end_addr;
    }
  }

  return 0;
}
