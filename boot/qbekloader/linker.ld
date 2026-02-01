OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
    . = 0x00008800;
    .text ALIGN(CONSTANT(MAXPAGESIZE)) :
    {
        .text.qbekloader_main
        *(.text)
        *(.text.*)
    }

    .rodata ALIGN(CONSTANT(MAXPAGESIZE)) :
    {
        *(.rodata)
        *(.rodata.*)
    }

    .data ALIGN(CONSTANT(MAXPAGESIZE)) :
    {
        start_ctors = .;
        *(.ctor*)
        end_ctors = .;
        start_dtors = .;
        *(.dtor*)
        end_dtors = .;
        *(.data)
    }

    .bss ALIGN(CONSTANT(MAXPAGESIZE)) :
    {
        _sbss = .;
        *(COMMON)
        *(.bss)
        _ebss = .;
    }

   end = .; _end = .; __end = .;
}
