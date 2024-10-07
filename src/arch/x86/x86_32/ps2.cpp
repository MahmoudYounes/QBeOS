#include "arch/x86/include/ps2.h"

PS2::PS2(){
  kprint("initializing PS2\n\0");
  //initialize();
}

void PS2::initialize(){
  // Initialize usb controllers to disable USB Legacy Mode uses PCI/PCIe
  // PCI.DisableUSBLegacy();
  
  // See if PS2 Controller exists. USE ACPI FADT
  if(!acpi.IsPS2Supported()) {
    // looks like an issue with Qemu bios?
    kprint("couldn't detect PS2 controller from ACPI. Continue anyway\n\0");
  }

  // First we want to do controller self test
  // we disable interrupts, and translation, and enable clocks
  configure();
  readData(NO_WAIT_READY);

  // Disable all PS/2 Ports
  disablePorts();
  
  readData(NO_WAIT_READY);
  readData(NO_WAIT_READY);
  readStatus();
  readData(NO_WAIT_READY);
  
  // Flush the output buffer
  FlushOutput();
  
  selfTest();
  if (!testPassed){
    initialized = false;
    kprint("not initializing ps2. self test failed\n\0");
  }

  // Second we want to detect if we are a single or dual port controller
  // Enalbe all clocks, make sure interrupts and translation are disabled
  configure();
  
  // Disable the devices
  disablePorts();

  // if port 2 is still enabled then no port2
  detectChannel2();

  // test the interfaces
  interfaceTest();
  if (!port2TestPass){
    hasTwoPorts = false;
  }

  // final phase
  configure();

  // enable the ports available 
  enablePorts();

  // disallow all devices from sending data until their drivers are inplace
  disableScanning();

  if (!port1TestPass){
    kprint("PS2 controller didn't pass initialization. assuming no PS2\n\0");
    initialized = false;
  } else {
    kprint("PS2 Controller Initialized\n\0");
    initialized = true;
  }


}

void PS2::disableScanning(){
  writePort1(DISABLE_DEV);
  if (hasTwoPorts){
    writePort2(DISABLE_DEV);
  }
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

void PS2::disablePorts() {
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
    return ERRNO_RDATA;
  }

  return inb(DATA_PORT);
}

void PS2::FlushOutput(){
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
 * so far we have write the cfg byte to enable clocks for both ports.
 * then we have disabled both devices. by default any PS2 controller
 * would support at least one port. if a port is not supported
 * and we enabled the clock and disabled the devices then
 * the controller would disable the clock of the device disabled if 
 * it supports it.
 * hence if we check now on the clock of the second port and found it
 * still enabled, after we disabled the device, that means this 
 * device is not supported;
 *
*/
void PS2::detectChannel2(){
  uint8_t testRes;
 
  writeCommand(READ_CFG_BYTE);
  testRes = readData(WAIT_READY);
  if (!(testRes & (1 << 4))){
    kprint("Controller doesn't have two ports\n\0");
    hasTwoPorts = false;
  } else {
    kprint("Controller has two ports\n\0");
    hasTwoPorts = true;
  }
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

void PS2::enablePorts(){
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
  uint8_t buf;
  FlushOutput();
retry:
  writeData(data);
  buf = readData(WAIT_READY);
  switch(buf){
    case ACK:
      return;
    case RESEND:
      goto retry;
    default:
      return; // TODO: add err
  }
}

void PS2::writePort2(uint8_t data){
  uint8_t buf;
  FlushOutput();
retry:
  writeCommand(WRITE_PORT2);
  writeData(data);
  buf = readData(WAIT_READY);
  switch(buf){
    case ACK:
      return;
    case RESEND:
      goto retry;
    default:
      return; // TODO: add err
  }

}

void PS2::resetPort1(){
  int8_t res, trials = MAX_TRIALS;
  uint8_t data, nextCmp = 0xfa;

pulsewrite:
  writePort1(RESET);
  res = readData(WAIT_READY);
  if (res == ERRNO_RDATA){
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
  if (res == ERRNO_RDATA){
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
  
  FlushOutput();
}

void PS2::resetPC(){
  writeCommand(0xfe); 
}

void PS2::EnableInterrupt1(){
  uint8_t cfg;
  
  FlushOutput();
  writeCommand(READ_CFG_BYTE);
  cfg = readData(WAIT_READY);

  cfg |= 1;
  writeCommand(WRITE_CFG_BYTE);
  writeData(cfg);
}

uint8_t PS2::WriteCommand(uint8_t cmd, enum PORT port){
  switch (port){
    case PORT1:
      writePort1(cmd);
      break;
    case PORT2:
      writePort2(cmd);
      break;
  }
  return 0;
}

uint8_t PS2::ReadData(){
  return readData(WAIT_READY);
}
