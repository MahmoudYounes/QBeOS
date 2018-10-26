OUTPUT_FORMAT(elf32-i386)
ENTRY(BeOSKernel)

SECTIONS
{
    . = 0x9000;
    .text : { *(.text) }
    .data : { *(.data) }
    .bss  : { *(.bss) *(COMMON) }
}