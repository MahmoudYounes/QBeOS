#ifndef HASH_H
#define HASH_H

#include "include/common.h"


class Hasher{
public:
  Hasher(){};
  virtual uint32_t Hash(uint32_t item)=0;
  virtual uint32_t Hash(uint64_t item)=0;
  virtual uint32_t Hash(char *item)=0;
  virtual uint32_t Hash(char item)=0;

};

#endif
