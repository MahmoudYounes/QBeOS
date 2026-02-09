#include "include/common.h"
#include "include/pcie.h"
#include "include/screen.h"
#include "include/boothdr.h"
#include "include/memory.h"

void panic(char str[]){
  putstr(str);
  HLT();
}

void qbekloader_main(){
  init_screen();
  clear_screen();
  parse_boot_hdr();

  init_memory();
  init_pci();

  static char str[] = "Welcome to qbek\0"; 
  putstr(str);
    
  HLT();
}
