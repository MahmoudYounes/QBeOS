#ifndef PIC_H
#define PIC_H

#include "include/common.h"
#include "include/logger.h"

#define MASTER_CMDPORT 0x20
#define MASTER_DPORT   0x21

#define SLAVE_CMDPORT  0xa0
#define SLAVE_DPORT    0xa1

#define EOI 0x20

// By default we are disabling PIC
class PIC {
 
public:
  PIC();

  void DisablePIC();
  void SendEOI();
};

#endif /* PIC_H */
