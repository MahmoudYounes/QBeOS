OUTPUT_FORMAT(elf32-i386)
ENTRY(boot2)

SECTIONS
{
    . = 0x9000;
    .text : { *(.text) }
    .data : { *(.data) }
    .bss  : { *(.bss) *(COMMON) }
}