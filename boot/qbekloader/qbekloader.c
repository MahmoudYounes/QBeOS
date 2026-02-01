#include "include/common.h"
#include "include/screen.h"

void qbekloader_main(){
  init_screen();
  clear_screen();

  static char str[] = "Welcome to qbek\0"; 
  putstr(str);

  HLT();
}
