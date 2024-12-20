#include "arch/x86/include/ps2.h"

PS2::PS2(){
  kprint("initializing PS2\n\0");
  initialize();
}

void PS2::initialize(){ 
   // Initialize usb controllers to disable USB Legacy Mode uses PCI/PCIe
  // PCI.DisableUSBLegacy();
  
  // See if PS2 Controller exists. USE ACPI FADT
  //if(!acpi.IsPS2Supported()) {
    // looks like an issue with Qemu bios?
  //  kprint("couldn't detect PS2 controller from ACPI. Continue anyway\n\0");
  //}
    
  // Disable all PS/2 Ports
  disablePorts();
  FlushOutput();


  // First we want to do controller self test
  // we disable interrupts, and translation, and enable clocks
  configure();
  
  selfTest();
  if (!testPassed){
    initialized = false;
    panic("not initializing ps2. self test failed\n\0");
  }

  // if port 2 is still enabled then no port2
  detectChannel2();

  // test the interfaces
  interfaceTest();
  if (!port2TestPass){
    hasTwoPorts = false;
  }

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
  uint8_t portData;
  portData = DISABLE_DEV;
  writePort1(&portData);
  if (hasTwoPorts){
    portData = DISABLE_DEV;
    writePort2(portData);
  }
}

uint8_t PS2::readStatus(){
  return inb(CMD_PORT);
}

bool PS2::canWrite(){
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
  while (!canWrite()){}
 
  // write the command
  outb(CMD_PORT, cmd);
}

void PS2::disablePorts() {
  writeCommand(DISABLE_PORT1);
  writeCommand(DISABLE_PORT2);
}

bool PS2::canRead(){
  uint8_t status;

  // check bit 0 is set in STATUS_REG(Read CMD_PORT)
  status = readStatus(); 
  if (status & 1){
    return true;
  }
  return false;
}

int8_t PS2::readData(bool wait, uint8_t *buf){
  // wait until data is ready
  int32_t timeout = 1000;

  while (wait && !canRead() && timeout > 0) {timeout--;}
  if (timeout == 0){
    return ERR_TIMEOUT;    
  }

  *buf = inb(DATA_PORT);
  return 0;
}

void PS2::FlushOutput(){
  //if (canReadData()){
    inb(DATA_PORT);
  //}
}

void PS2::writeData(uint8_t data){
  while (!canWrite()){}

  // write the data
  outb(DATA_PORT, data);
}

void PS2::configure(){
  // doing the configuration, we only do that for port 1 just to make sure
  // the controller is in a known state, then we need to reconfigure things
  // for port 2 as well.
  
  uint8_t cfgByte;
  int8_t err;
  
  writeCommand(READ_CFG_BYTE);
  err = readData(WAIT_READY, &cfgByte);
  if (err){
    kprintf("error while reading configuration byte: %d\n\0", err);
    return;
  }
  
  if (cfgByte & 4){
    kprint("PS2 POST Checks passed\n\0");
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
  int8_t err;

  writeCommand(SELF_TEST);
  while (true){
    err = readData(WAIT_READY, &testRes);
    if (err) {
      kprintf("selftest failed with err: %d\n\0", err);
      return;
    }
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
  int8_t err;
 
  writeCommand(READ_CFG_BYTE);
  err = readData(WAIT_READY, &testRes);
  if (err){
    kprintf("failed to detect channel2 with err: %d\n\0", err);
    return;
  }
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
  int8_t err;

  writeCommand(TEST_PORT1);
  while (true){
    err = readData(WAIT_READY, &testRes);
    if (err){
      kprintf("ps2 interface test failed with err: %d\n\0", err);
      return;
    }
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
    err = readData(WAIT_READY, &testRes);
    if (err) {
      kprintf("ps2 interface test failed with err: %d\n\0", err);
      return;
    }
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

void PS2::writePort1(uint8_t *data){
  uint8_t buf;
  int8_t err;
  uint32_t output_buf, output_data;
retry:
  writeData(*data);
  err = readData(WAIT_READY, &buf);
  if (err){
    kprintf("failed to write to port1 with err: %d\n\0", err);
    return;
  }
  switch(buf){
    case ACK:
      *data = ACK;
      return;
    case RESEND:
      *data = RESEND;
      goto retry;
    case ERRIBUF:
       kprint("kbd internal buffer overrun\n\0");
    default:
      *data = buf;
      output_buf = (uint32_t) buf;
      output_data = (uint32_t) data;
      kprintf("returning from writePort1 without any valid keyboard response. response is %x for data %x\n\0", output_buf, output_data);
      return; // TODO: add err
  }
}

void PS2::writePort2(uint8_t data){
  uint8_t buf;
  int8_t err;
retry:
  writeCommand(WRITE_PORT2);
  writeData(data);
  err = readData(WAIT_READY, &buf);
  if (err){
    kprintf("failed to write to port2 with err: %d\n\0", err);
  }
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
  int8_t trials = MAX_TRIALS, err;
  uint8_t data = RESET, nextCmp = 0xfa;

pulsewrite:
  writePort1(&data);

  err = readData(WAIT_READY, &data);
  if (err){
    if (trials == 0){
      goto check;
    }
    trials--;
    goto pulsewrite;
  }

check:
  if (data == 0xfa){
    nextCmp = 0xaa;
  } else if (data == 0xaa){
    nextCmp = 0xfa;
  }
  err = readData(WAIT_READY, &data);
  if (err){
    kprintf("failed to reset port1 with err: %d\n\0", err);
  }
  if (data != nextCmp){
    panic("failed to reset port1\n\0");
  }
}

void PS2::resetPort2(){
  int8_t err, trials = MAX_TRIALS;
  uint8_t data, nextCmp = 0xfa;

pulsewrite:
  writePort2(RESET);
  err = readData(WAIT_READY, &data);
  if (err){
    if (trials == 0){
      goto check;
    }
    trials--;
    goto pulsewrite;
  }

check:
  if (data == 0xfa){
    nextCmp = 0xaa;
  } else if (data == 0xaa){
    nextCmp = 0xfa;
  }
  
  err = readData(WAIT_READY, &data);
  if (err) {
    kprintf("failed to reset port2 with err: %d\n\0", err);
  }
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
  int8_t err;
  
  writeCommand(READ_CFG_BYTE);
  err = readData(WAIT_READY, &cfg);
  if (err){
    kprintf("failed to enable interrupt 1 with err: %d", err);
    return;
  }

  cfg |= 1;
  writeCommand(WRITE_CFG_BYTE);
  writeData(cfg);
}

uint8_t PS2::WriteCommand(uint8_t cmd, enum PORT port){
  uint8_t buf = cmd;

  switch (port){
    case PORT1:
      writePort1(&buf);
      return buf;
    case PORT2:
      writePort2(cmd);
      break;
  }
  return 0;
}

int8_t PS2::ReadData(uint8_t *buf){
  return readData(WAIT_READY, buf);
}

uint8_t PS2::ReadStatus(){
  return readStatus();
}
