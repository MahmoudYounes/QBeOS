#ifndef PS2_H
#define PS2_H

#include "acpi/include/acpi.h"
#include "include/common.h"

#define DATA_PORT 0x60
#define CMD_PORT 0x64
#define TEST_PASS 0x55
#define TEST_FAIL 0xfc
#define READ_CFG_BYTE 0x20
#define WRITE_CFG_BYTE 0x60
#define SELF_TEST 0xaa
#define RESEND 0xfe

#define PORT_TEST_PASS 0x0

#define DISABLE_PORT1 0xad
#define TEST_PORT1 0xab
#define ENABLE_PORT1 0xae
#define WRITE_PORT1 0xd2

#define DISABLE_PORT2 0xa7
#define TEST_PORT2 0xa9
#define ENABLE_PORT2 0xa8
#define WRITE_PORT2 0xd4


#define WAIT_READY true
#define NO_WAIT_READY false

#define RESET 0xff

#define MAX_TRIALS 5

#define ERRNO_RDATA -1

#define ECHO 0xee
#define ENABLE_DEV 0xf4
#define DISABLE_DEV 0xf5
#define ACK 0xfa


enum PORT{
  PORT1,
  PORT2
};

extern ACPIM acpi;

/**
  * the ps/2 is a controller chipset that drives 2 devices connected on ps/2 port.
  * normally these would be the mouse and the keyboard.
  * some motherboards emulate the usb input from mouse and keyboard as ps/2
  * mouse and keyboards (usb legacy mode). in this case, it's better to 
  * disable this mode and rely on PCI/Usb to control and drive these devices.
  * this is the controller not the devices themselves.
  * 
  * there is no direct indication that this chipset 8042 is tied to intel x86
  * pcs. however, I will go out on a limb and say that it is. for arm fx, PCI
  * is the defacto standard I believe;
  */
class PS2 {
private:
  void initialize();
  bool canWriteCommand();
  void writeCommand(uint8_t cmd);
  uint8_t readStatus();
  bool canReadData();
  int8_t readData(bool wait);
  bool canWriteData();
  void writeData(uint8_t data);
  void writePort1(uint8_t data);
  void writePort2(uint8_t data);
  void disablePorts();
  void enablePorts();
  void configure();
  void selfTest();
  void detectChannel2();
  void testPorts();
  void interfaceTest();
  void resetDevices();
  void resetPort1();
  void resetPort2();
  void resetPC();
  void disableScanning();
 
  bool testPassed;
  bool port1TestPass;
  bool port2TestPass;
  bool initialized;
  bool hasTwoPorts;
public:
  PS2();

  void EnableInterrupt1();
  void EnableInterrupt2();

  uint8_t WriteCommand(uint8_t cmd, enum PORT port);
  int8_t ReadData();
  void FlushOutput();
  uint8_t ReadStatus();

};

inline PS2 ps2;

#endif
