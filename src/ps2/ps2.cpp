#include "ps2/include/ps2.h"

PS2::PS2(){
  uint8_t psbuf;
  // Initialize usb controllers to disable USB Legacy Mode uses PCI/PCIe
  // PCI.DisableUSBLegacy();
  
  // See if PS2 Controller exists. USE ACPI FADT
  if(!acpi.IsPS2Supported()) {
    kprint("couldn't detect PS2 controller from ACPI\n\0");
  }
 
  configure();
  psbuf = readData(NO_WAIT_READY);

  // Disable all PS/2 devices
  disableDevices();
  
  psbuf = readData(NO_WAIT_READY);
  psbuf = readData(NO_WAIT_READY);
  psbuf = readStatus();
  psbuf = readData(NO_WAIT_READY);
  
  // Flush the output buffer
  flushOutput();

  selfTest();
  if (!testPassed){
    initialized = false;
    kprint("not initializing ps2. self test failed\n\0");
  }

  detectChannel2();

  interfaceTest();

  enableDevices();

  resetDevices();

  kprint("PS2 Controller Initialized\n\0");

  testKeyboard();
}

uint8_t PS2::readStatus(){
  return inb(CMD_PORT);
}

bool PS2::canWriteCommand(){
  uint8_t status;

  // check bit 1 is clear in STATUS_REG(Read CMD_PORT)
  status = readStatus();
  if (status & 2){
    return false; 
  }
  return true;
}

void PS2::writeCommand(uint8_t cmd){ 
  // wait for controller to be ready
  while (!canWriteCommand()){}
 
  // write the command
  outb(CMD_PORT, cmd);
}

void PS2::disableDevices() {
  writeCommand(DISABLE_PORT1);
  writeCommand(DISABLE_PORT2);
}

bool PS2::canReadData(){
  uint8_t status;

  // check bit 0 is set in STATUS_REG(Read CMD_PORT)
  status = readStatus(); 
  if (status & 1){
    return true;
  }
  return false;
}

int8_t PS2::readData(bool wait){
  // wait until data is ready
  int32_t timeout = 1000;
  while (wait && !canReadData() && timeout > 0) {timeout--;}
  if (timeout == 0){
    return NO_RDATA;
  }

  return inb(DATA_PORT);
}

void PS2::flushOutput(){
  inb(DATA_PORT);
}

bool PS2::canWriteData(){
  return canWriteCommand();
}

void PS2::writeData(uint8_t data){
  while (!canWriteData()){}

  // write the data
  outb(DATA_PORT, data);
}

void PS2::configure(){
  // doing the configuration, we only do that for port 1 just to make sure
  // the controller is in a known state, then we need to reconfigure things
  // for port 2 as well.
  
  uint8_t cfgByte;
  
  writeCommand(READ_CFG_BYTE);
  cfgByte = readData(WAIT_READY);
  
  if (cfgByte & 4){
    panic("os Shouldn't be running?\n\0");
  }

  // Disable interrupts clear bit 0
  cfgByte = cfgByte & 0xfe;

  // Disable translation clear bit 6
  cfgByte = cfgByte & 0xbf;

  // Enable port 1 clock clear bit 4
  cfgByte = cfgByte & 0xef;

  // Enable port 2 clock clear bit 5
  cfgByte = cfgByte & 0xdf;

  writeCommand(WRITE_CFG_BYTE);
  writeData(cfgByte);
 }

void PS2::selfTest(){
  uint8_t testRes;

  writeCommand(SELF_TEST);
  while (true){
    testRes = readData(WAIT_READY);
    switch (testRes){
      case RESEND:
        writeCommand(SELF_TEST);
        continue;
      case TEST_FAIL:
        testPassed = false;
        break;
      case TEST_PASS:
        testPassed = true;
        break;
    }
    break;
  }
}


/**
 * try enable second port. if there is 2 channels then bit 5 should be cleared
 *
*/
void PS2::detectChannel2(){
  uint8_t testRes;

  writeCommand(ENABLE_PORT2);
  
  writeCommand(READ_CFG_BYTE);
  testRes = readData(WAIT_READY);
  if (!(testRes & 1 << 4)){
    hasTwoPorts = true;
    return;
  }
  kprint("Controller doesn't have two ports\n\0");
  // TODO: PS/2 controllers more likely than not support 2 ports.
  // and most PCs have 2 channels. add the try again logic.
}

void PS2::testPorts(){
  writeCommand(TEST_PORT1);

}

void PS2::interfaceTest(){
  uint8_t testRes;

  writeCommand(TEST_PORT1);
  while (true){
    testRes = readData(WAIT_READY);
    switch (testRes){
      case RESEND:
        writeCommand(TEST_PORT1);
        continue;
      case PORT_TEST_PASS:
        port1TestPass = true;
        break;
      default:
        port1TestPass = false;
        break;
    }
    break;
  }

  writeCommand(TEST_PORT2);
  while (true){
    testRes = readData(WAIT_READY);
    switch (testRes){
      case RESEND:
        writeCommand(TEST_PORT2);
        continue;
      case PORT_TEST_PASS:
        port2TestPass = true;
        break;
      default:
        port2TestPass = false;
        break;
    }
    break;
  }
}

void PS2::enableDevices(){
  writeCommand(ENABLE_PORT1);
  if (hasTwoPorts){
    writeCommand(ENABLE_PORT2);
  }
}

void PS2::resetDevices(){
  resetPort1();
  resetPort2();
}

void PS2::writePort1(uint8_t data){
  writeData(data);
}

void PS2::writePort2(uint8_t data){
  writeCommand(WRITE_PORT2);
  writeData(data);
}

void PS2::resetPort1(){
  int8_t res, trials = MAX_TRIALS;
  uint8_t data, nextCmp = 0xfa;

pulsewrite:
  writePort1(RESET);
  res = readData(WAIT_READY);
  if (res == NO_RDATA){
    if (trials == 0){
      goto check;
    }
    trials--;
    goto pulsewrite;
  }

check:
  data = (uint8_t) res; 
  if (data == 0xfa){
    nextCmp = 0xaa;
  } else if (data == 0xaa){
    nextCmp = 0xfa;
  }
  res = readData(WAIT_READY);
  data = (uint8_t) res;
  if (data != nextCmp){
    panic("failed to reset port1\n\0");
  }
}

void PS2::resetPort2(){
  int8_t res, trials = MAX_TRIALS;
  uint8_t data, nextCmp = 0xfa;

pulsewrite:
  writePort2(RESET);
  res = readData(WAIT_READY);
  if (res == NO_RDATA){
    if (trials == 0){
      goto check;
    }
    trials--;
    goto pulsewrite;
  }

check:
  data = (uint8_t) res;
  if (data == 0xfa){
    nextCmp = 0xaa;
  } else if (data == 0xaa){
    nextCmp = 0xfa;
  }
  
  res = readData(WAIT_READY);
  data = (uint8_t)res;
  if (data != nextCmp){
    panic("failed to reset port2\n\0");
  }
  
  flushOutput();
}

void PS2::testKeyboard(){
  int8_t ichar;
  char buf[255];
  int32_t testing = 1000000;

  while (true){
    if (testing-- == 0) {
      break;
    }

    if (!canReadData()) {
      continue;
    }
    
    ichar = readData(NO_WAIT_READY);
    if (ichar == NO_RDATA) {
      continue;
    }
    kprintf(buf, "read %d\n\0", ichar);
  }
}
