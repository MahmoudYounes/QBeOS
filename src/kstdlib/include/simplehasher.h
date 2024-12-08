#ifndef SIMPLE_HASHER_H
#define SIMPLE_HASHER_H

#include "kstdlib/include/hash.h"

class SimpleHasher : public Hasher {
  // simple hasher is something % slotsCount
private:
  // this is the number w
  uint64_t slotsCount;
public:
  SimpleHasher(uint64_t sc);
  void *Hash(uint8_t *msg, uint64_t msgSize);
};

#endif
