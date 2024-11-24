#ifndef SIMPLE_HASHER_H
#define SIMPLE_HASHER_H

#include "kstdlib/include/hash.h"

class SimpleHasher : public Hasher {
  // simple hasher is something % slotsCount
private:
  // this is the number w
  uint32_t slotsCount;
public:
  SimpleHasher(uint32_t sc);
  uint32_t Hash(uint32_t item);
  uint32_t Hash(uint64_t item);
  uint32_t Hash(char *item);
  uint32_t Hash(char item);
};

#endif
