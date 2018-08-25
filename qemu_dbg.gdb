add-symbol-file build/bootloader.elf 0x7c00
add-symbol-file build/boot.elf 0x9000
target remote | qemu-system-i386 -S -gdb stdio -m 32 -boot d -cdrom bin/BeOs.iso
set architecture i8086
set disassembly-flavor intel
layout asm
break *0x7c00