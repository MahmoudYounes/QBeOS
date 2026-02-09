func_MarkBootHdrComplete:
    pushad

    xor ebx, ebx
    mov bx, [BootHDRAddress]
    mov es, bx
    xor di, di
    add di, 10
    mov eax, 0x55be
    stosd

    popad
    ret
