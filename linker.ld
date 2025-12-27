OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
    . = 0x0000a400;
    .text :
    {
        *(.text.*kmain*)
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
        start_ctors = .;
        *(.ctor*)
        end_ctors = .;
        start_dtors = .;
        *(.dtor*)
        end_dtors = .;
        *(.data)
    }

    .bss  :
    {
        _sbss = .;
        *(COMMON)
        *(.bss)
        _ebss = .;
    }

   end = .; _end = .; __end = .;
}
