OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
    . = 0x00100000;
    .text : { *(.text) }
    .data : { *(.data) }
    .bss  : { *(.bss) *(COMMON) }
}
