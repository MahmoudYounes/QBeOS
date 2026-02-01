OUTPUT_FORMAT(elf32-i386)
ENTRY(qbekloader_main) 

SECTIONS
{
  . = 0x00008800;
  .text :
    {
      *(.text.qbekloader_main)
      *(.text)
      *(.text.*)
    }

    .rodata :
    {
        *(.rodata)
        *(.rodata.*)
    }

    .data :
    {
      *(.data .data.*)
    }

    __bss_start = .;
    .bss (NOLOAD) : ALIGN(16) {
      *(.bss .bss.*)
      _sbss = .;
      *(COMMON)
      _ebss = .;
    }
    __bss_end = .;

    end = .; _end = .; __end = .; 
}

    
