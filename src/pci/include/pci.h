#ifndef PCI_H
#define PCI_H

#include "include/common.h"
#include "include/kargs.h"

#define CONFIG_ADDR 0xcf8
#define CONFIG_DATA 0xcfc

// class that manages and interacts with pci bus if it exists in the system.
// this is legacy PCI
class PCI {
private:
  void enumeratePCIDevices();
  uint32_t readConf(uint32_t busNum, uint32_t deviceNum, uint32_t funcNum, uint32_t regOffset);
  uint8_t readConfByte(uint32_t busNum, uint32_t deviceNum, uint32_t funcNum, uint32_t regOffset);
  uint16_t readConfWord(uint32_t busNum, uint32_t deviceNum, uint32_t funcNum, uint32_t regOffset);
public:
  PCI(kargs *args);
};

#endif
