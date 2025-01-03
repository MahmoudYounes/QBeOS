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
#define ICW2M 0x30 // interrupt begin
#define ICW2S 0x38 // interrupt begin
#define ICW3M 0x04
#define ICW3S 0x02
#define ICW4 0x01
#define EOI 0x20

#define READ_ISR 0x0b
#define READ_IRR 0x0a

// By default we are disabling PIC
class PIC {
private:
    uint8_t getIMRM();
  uint8_t getIMRS();

public:
  PIC();
  void Initialize();

  void STI();
  void CLI();
  void DisablePIC();
  int8_t EnableInterrupt(uint8_t irqn);
  int8_t DisableInterrupt(uint8_t irqn);
  void SendEOI(uint8_t);
  uint16_t GetISR();
  uint16_t GetIRR();
};

inline PIC pic;

#endif /* PIC_H */
