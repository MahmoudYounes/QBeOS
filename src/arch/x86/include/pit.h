#ifndef PIT_H
#define PIT_H

#include "include/common.h"
#include "include/logger.h"

#define CHAN0 0x40
#define CHAN1 0x41
#define CHAN2 0x42
#define CMDCH 0x43

#define CHANL0 0x00
#define CHANL1 0x40
#define CHANL2 0x80
#define RDBACK 0xc0

#define LOHIB  0x30

#define TERMC  0x0

#define BIN    0x0

#define DIVISOR 0x1

class PIT {
public:
  PIT();
  void Reload();
};

inline PIT pit;

#endif
