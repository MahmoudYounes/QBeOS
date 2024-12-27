#ifndef MEM_REGION_H
#define MEM_REGION_H

#include "include/common.h"

#define IS_SYS_REGION(memPtr) memPtr->state != usable

enum memState {
  invalid,
  usable,
  reserved,
  acpiReclaim,
  acpiNVS,
  badMem,
  // any new types additions start here
  KERN,
};

// class that holds physical memory region info
class MemoryRegion {
public:
  // where the page frame starts
  uint8_t *baseAddress;
  // the size of the page
  uint64_t size;
  // the id of the region this page came from at boot time
  uint64_t bootRegionID;
  // the current region id
  uint64_t regionID;
  // when a request is made, it is associated with an incremental allocation id
  // this is used to keep track of all the pages that were allocated together
  // at the same time to help free them together.
  uint64_t allocRequestID = 0;
  enum memState state;
  // pointer to the next page
  MemoryRegion *next;

  long GetSize();
};

#endif /* MEM_REGION_H */
