#include "pci/include/pci.h"

PCI::PCI(){
  enumeratePCIDevices();
}

void PCI::enumeratePCIDevices(){
   
}

uint32_t PCI::readConf(uint32_t busNum, uint32_t deviceNum, uint32_t funcNum, uint32_t regOffset){
  uint32_t addr = 0;

  // only first 8 bits are valid register offset. the two lowest bits must be set to zero.
  regOffset = regOffset & 0xfc;

  // only first 3 bits are valid function number
  funcNum = funcNum & 7;

  // only first 5 bits are valid device number
  deviceNum = deviceNum & 0x1f;

  // only first 8 bits are valid bus number
  busNum = busNum & 0xff;

  // construct the address
  addr = (1 << 31) + (busNum << 16) + (deviceNum << 11) + (funcNum << 8) + regOffset;

  // output the address we want to read on the address port
  outl(CONFIG_ADDR, addr);
  
  // read the config data from the config data port
  return inl(CONFIG_DATA);
}

uint8_t PCI::readConfByte(uint32_t busNum, uint32_t deviceNum, uint32_t funcNum, uint32_t regOffset){
  int configVal;

  configVal = readConf(busNum, deviceNum, funcNum, regOffset);

  // we have read 4 bytes we only need the lowest 1 byte. 
  return configVal & 0xff; 
}

uint16_t PCI::readConfWord(uint32_t busNum, uint32_t deviceNum, uint32_t funcNum, uint32_t regOffset){
  int configVal;

  configVal = readConf(busNum, deviceNum, funcNum, regOffset);

  // we have read 4 bytes we only need the lowest 1 byte. 
  return configVal & 0xffff; 
}



