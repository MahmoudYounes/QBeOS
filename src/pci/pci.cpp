#include "pci/include/pci.h"

PCI::PCI(){}

PCI::PCI(kargs *args){
  if (!args->pciSupported){
    panic("PCI is not supported\n\0");
  }

  if (args->pciConfigMech != 1){
    panic("PCI Config mechanism 1 is not supported\n\0");
  }

  enumeratePCIDevices();
}

void PCI::enumeratePCIDevices(){
  uint16_t val;
  Device device;
  devices = new Device[1]();
  
  for (int busNum = 0; busNum < 0xff; busNum++){
    for (int devNum = 0; devNum < 0x1f; devNum++){
        val = readConf(busNum, devNum, 0, 0);
        if (val == 0xffff){
          continue;
        } else {
          parseDevice(busNum, devNum, &device); 
          kprintf("found device %d on bus %d with vendorID %x and deviceID %x\n\0", devNum, busNum, device.vendorID, device.deviceID); 
        }
    }
  }

  kprint("finished pci enum\n\0"); 
}

void PCI::parseDevice(uint32_t busNum, uint32_t devNum, Device *dev){
  uint32_t val;

  val = readConf(busNum, devNum, 0, 0);

  dev->busNum = busNum;
  dev->devNum = devNum;
  dev->vendorID = val & 0xffff;
  dev->deviceID = val >> 16;
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
