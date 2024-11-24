#include <kstdlib/include/simplehasher.h>

SimpleHasher::SimpleHasher(uint32_t sc){
  slotsCount = sc;
}

uint32_t SimpleHasher::Hash(uint32_t item){
  return item % slotsCount; 
}

uint32_t SimpleHasher::Hash(uint64_t item){
  return item % slotsCount;
}

uint32_t SimpleHasher::Hash(char item){
  return item % slotsCount;
}

uint32_t SimpleHasher::Hash(char *item){
  uint32_t charSum = 0;
  for (char *ptr = item; *ptr != '\0'; ptr++){
    charSum += *ptr;
  }

  return charSum % slotsCount;
}
