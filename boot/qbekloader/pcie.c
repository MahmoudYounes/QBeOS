#include "include/common.h"
#include "include/boothdr.h"
#include "include/pcie.h"
#include "include/memory.h"
#include "include/format.h"
#include "include/screen.h"

#define HEADER_OFFSET 0xc
#define HEADER_SHFT 16 
#define MF_HEADER_MASK 0x80

#define CLASS_SUBCLASS_OFFSET 0x8
#define CLASS_SUBCLASS_SHFT 16
#define CLASS_MASK 0xff000000
#define SUBCLASS_MASK 0xff0000

#define CLS_MASS_STORAGE 0x1
#define SCLS_SCSI        0x0
#define SCLS_IDE         0x1
#define SCLS_FLOPPY      0x2
#define SCLS_IPI         0x3
#define SCLS_RAID        0x4
#define SCLS_ATA         0x5
#define SCLS_SATA        0x6
#define SCLS_SSCSI       0x7
#define SCLS_NVMC        0x8
#define SCLS_OTHER       0x80

#define HDR_GENERIC      0x0
#define HDR_PCI_BRIDGE   0x1
#define HDR_PCI_CARD     0x2

char panic_msg[] = "PCI is not supported\n\0"; 

static struct device_t* devices[MAX_DEV_COUNT];
static int last_dev_idx;

// PCI config methods
static uint32_t read_conf(uint8_t busnr, uint8_t devnr, uint8_t funcnr, uint8_t regoff);

// config methods
static uint8_t get_header_type(uint8_t busnr, uint8_t devnr, uint8_t funcnr);
static bool is_multifunc(uint8_t header);
static uint16_t get_class_subclass(uint8_t busnr, uint8_t devnr, uint8_t funcnr);

// device methods
static void enumerate_devices();
static void configure_generic(uint8_t busnr, uint8_t devnr, uint8_t funcnr, struct device_t *dev);
static void configure_bridge(uint8_t busnr, uint8_t devnr, uint8_t funcnr, struct device_t *dev);
static void configure_card(uint8_t busnr, uint8_t devnr, uint8_t funcnr, struct device_t *dev);


// debug
static void print_debug();

/*
  * PCIE is required to provide config access mechanism 1 for backwards comp.
  * We don't need to do an ACPI handling in this situation.
  */
void init_pci(){
  if (!is_pcie_supported()){
    panic(panic_msg);
  }
  
  last_dev_idx = 0;
  enumerate_devices();
  print_debug();
}

void enumerate_devices(){
  uint32_t val;
  for (int busNum = 0; busNum <= 0xff; busNum++){
    for (int devNum = 0; devNum <= 0x1f; devNum++){
      val = read_conf(busNum, devNum, 0, 0);
      if ((val & 0xffff) == 0xffff){
        continue;
      }
      add_device(busNum, devNum, 0, val);

      uint8_t headertype = get_header_type(busNum, devNum, 0);
      if (is_multifunc(headertype)){
        for (int funcNum = 1; funcNum < 8; funcNum++){
          val = read_conf(busNum, devNum, funcNum, 0);
          if ((val & 0xffff) == 0xffff){
            continue;
          }
          add_device(busNum, devNum, funcNum, val); 
        }
      }
    }
  }
}

void add_device(uint8_t busnr, uint8_t devnr, uint8_t funcnr, uint32_t val){
  struct device_t *dev = (struct device_t *)malloc(sizeof(struct device_t)); 
  dev->busnr = busnr;
  dev->devnr = devnr;
  dev->vendorid = val & 0xffff;
  dev->deviceid = val >> 16; 
  dev->funcnr = funcnr;

  // continue device configuration
  uint32_t class_subclass = get_class_subclass(busnr, devnr, funcnr);
  dev->classid = (class_subclass >> 8);
  dev->subclass = class_subclass & 0xff;

  uint8_t headertype = get_header_type(busnr, devnr, funcnr);
  dev->headertype = headertype; 

  switch (headertype & 0x7f){
    case HDR_GENERIC:
      configure_generic(busnr, devnr, funcnr, dev);
      break;
    case HDR_PCI_BRIDGE:
      configure_bridge(busnr, devnr, funcnr, dev);
      break;
    case HDR_PCI_CARD:
      configure_card(busnr, devnr, funcnr, dev);
      break;
    default:
      panic("unsupported pci device type\n\0");
  }

  devices[last_dev_idx++] = dev;
}

uint32_t read_conf(uint8_t busnr, uint8_t devnr, uint8_t funcnr, uint8_t regoff){
  uint32_t addr = 0;

  // only first 8 bits are valid register offset. the two lowest bits must be set to zero.
  regoff = regoff & 0xfc;

  // only first 3 bits are valid function number
  funcnr = funcnr & 7;

  // only first 5 bits are valid device number
  devnr = devnr & 0x1f;

  // only first 8 bits are valid bus number
  busnr = busnr & 0xff;

  // construct the address
  addr = (1 << 31) | (busnr << 16) | (devnr << 11) | (funcnr << 8) | regoff;

  // output the address we want to read on the address port
  outl(CONFIG_ADDR, addr);
  
  // read the config data from the config data port
  return inl(CONFIG_DATA);
}

uint8_t get_header_type(uint8_t busnr, uint8_t devnr, uint8_t funcnr){
  uint32_t val = read_conf(busnr, devnr, funcnr, HEADER_OFFSET);
  uint8_t tmp = val >> HEADER_SHFT;
  return tmp;
}

bool is_multifunc(uint8_t headertype){
  return headertype & MF_HEADER_MASK;
}

uint16_t get_class_subclass(uint8_t busnr, uint8_t devnr, uint8_t funcnr){
  uint32_t val = read_conf(busnr, devnr, funcnr, CLASS_SUBCLASS_OFFSET);
  return val >> CLASS_SUBCLASS_SHFT;
}

void print_debug(){
  static char buf[128];
  for (int idx = 0; idx < last_dev_idx; idx++){
    if (devices[idx] == NULL) {
      continue;
    }
    struct device_t* dev = devices[idx]; 
    sprintf(buf, "[PCIe] %x:%x.%x found device venid %x devid %x class %x subclass %x\n\0",
            dev->busnr,
            dev->devnr,
            dev->funcnr,
            dev->vendorid,
            dev->deviceid,
            dev->classid,
            dev->subclass);

    putstr(buf);
  }
}

void configure_generic(uint8_t busnr, uint8_t devnr, uint8_t funcnr, struct device_t *dev){
  uint8_t regoff_idx = 0x10;
  uint32_t val;

  val = read_conf(busnr, devnr, funcnr, regoff_idx);
  dev->gen_bar0addr = val;
  regoff_idx += 4;

  val = read_conf(busnr, devnr, funcnr, regoff_idx);
  dev->gen_bar1addr = val;
  regoff_idx += 4;

  val = read_conf(busnr, devnr, funcnr, regoff_idx);
  dev->bar2addr = val;
  regoff_idx += 4;

  val = read_conf(busnr, devnr, funcnr, regoff_idx);
  dev->bar3addr = val;
  regoff_idx += 4;

  val = read_conf(busnr, devnr, funcnr, regoff_idx);
  dev->bar4addr = val;
  regoff_idx += 4;

  val = read_conf(busnr, devnr, funcnr, regoff_idx);
  dev->bar5addr = val;
  regoff_idx += 4;

  val = read_conf(busnr, devnr, funcnr, regoff_idx);
  dev->cisptr = val;
  regoff_idx += 4;

  val = read_conf(busnr, devnr, funcnr, regoff_idx);
  dev->subsys_vendor = val & 0xffff;
  dev->subsys_id = val >> 16;
  regoff_idx += 4;
  
  val = read_conf(busnr, devnr, funcnr, regoff_idx);
  dev->exp_romaddr = val;
  regoff_idx += 4;

  val = read_conf(busnr, devnr, funcnr, regoff_idx);
  dev->g_res_capptr = val;
  regoff_idx += 8;

  val = read_conf(busnr, devnr, funcnr, regoff_idx);
  dev->g_intrline = val & 0xff;
  dev->g_intrpin = val >> 8;
  dev->mingrant = val >> 16;
  dev->maxlat = val >> 24;
}

void configure_bridge(uint8_t busnr, uint8_t devnr, uint8_t funcnr, struct device_t *dev){}

void configure_card(uint8_t busnr, uint8_t devnr, uint8_t funcnr, struct device_t *dev){}


