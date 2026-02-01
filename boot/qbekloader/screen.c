#include "include/common.h"
#include "include/screen.h"

static volatile uint16_t *VideoMemory; 

static int curpos;
static int rows;
static int cols;
static short format;
static char space;


void init_screen(){ 
  // disable the cursor
  outb(0x3d4, 0x0a);
  outb(0x3d5, 0x20);
  
  VideoMemory = (uint16_t *)0xb8000;
  
  curpos = 0;
  rows = 25;
  cols = 80;

  format = 0x2;
  space = 0x0020;
}

void clear_screen(){
  for (int i = 0; i < rows * cols; i++) {
    VideoMemory[i] = format << 8 | space;
  }
  curpos = 0;
}

void putstr(const char *str){
  for (const char *ci = str; *ci != '\0'; ci++) {
    putc(*ci);
  } 
}

void putc(char c){
  if (curpos == rows * cols) {
    scroll_up(); 
  }
  if (c == '\n') {
    int row = curpos / cols;
    curpos = (row + 1) * cols;
    return;
  }
  
  VideoMemory[curpos] = format << 8 | c;
  curpos++;
}

void scroll_up(){
  // scroll one line up
  int i = 0, j = cols;
  while (j < curpos) {
    VideoMemory[i] = VideoMemory[j];
    i++;
    j++;
  }

  while (i < curpos) {
    VideoMemory[i++] = format << 8 | space;
  }

  curpos = max(curpos - cols, 0);
};

