#ifndef PIC_H
#define PIC_H

#include "include/configs.h"
#include "include/common.h"
#include "include/logger.h"

#define MASTER_CMDPORT 0x20
#define MASTER_DPORT   0x21

#define SLAVE_CMDPORT  0xa0
#define SLAVE_DPORT    0xa1

#define ICW1 0x11
#define ICW2M 0x20
#define ICW2S 0x28
#define ICW3M 0x04
#define ICW3S 0x02
#define ICW4 0x01
#define EOI 0x20

// By default we are disabling PIC
class PIC {
private:
  void initialize();
  uint8_t getIMRM();
  uint8_t getIMRS();

public:
  PIC();
  void STI();
  void CLI();
  void DisablePIC();
  void SendEOI();
};

#endif /* PIC_H */
