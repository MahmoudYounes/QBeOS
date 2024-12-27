#ifndef HASH_H
#define HASH_H

#include "include/common.h"


class Hasher{
public:
  Hasher(){};
  virtual void *Hash(uint8_t *msg, uint64_t msgSize)=0;
};

#endif
