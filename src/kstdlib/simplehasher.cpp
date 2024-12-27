#include <kstdlib/include/simplehasher.h>

SimpleHasher::SimpleHasher(uint64_t sc){
  slotsCount = sc;
}

// SimpleHasher takes any msg and returns the module of that message in a 64
// bit buffer
void *SimpleHasher::Hash(uint8_t *msg, uint64_t msgSize){
  uint64_t buf, idx;
  uint8_t *ptr;

  buf = 0;
  ptr = msg;
  for (idx = 0; idx < msgSize; idx += 8){
    buf |= *ptr << idx;
    ptr++;
  }
  
  uint64_t *res = new uint64_t;
  *res = buf % slotsCount;
  return res; 
}
