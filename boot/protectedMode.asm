func_enableProtectedModeAndJmpKernel:
    
    ; assigning right address to gdt
    ; 4 GB size. 65535 (0xffff) entries * 64 bits per entry. each entry can describe: 32 bit address for segment + 20 bit offset)
    mov word [gdtrLimit], 0xffff
    ; offset: linear address of the table
    mov word [gdtrBaseLow], 0x0800
    mov word [gdtrBaseHigh], 0x0000	

    cld
    ; creating 3 entries in the GDT for now
    ; null descriptor
    mov ax, 0
    mov es, ax
    mov di, 0x0800
    mov cx, 4
    rep stosw

    ; code descriptor (4 GB)
    mov ax, 0xffff 	; limit 0 -> 15 = ffff
    stosw
    mov ax, 0		; base 16 -> 31 = 0
    stosw
    mov al, 0		; base 32 -> 39 = 0
    stosb
    mov al, 0x9a	; access byte 40 -> 47 = 9a = 10011100
    stosb
    mov al, 0xcf	; flags + limit 48 -> 55 = cf = 1100 1111
    stosb
    mov al, 0 		; base 56 -> 64 = 0 
    stosb

    ; data descriptor (4 GB)
    mov ax, 0xffff 	; limit 0 -> 15 = ffff
    stosw
    mov ax, 0		; base 16 -> 31 = 0
    stosw
    mov al, 0		; base 32 -> 39 = 0
    stosb
    mov al, 0x92	; access byte 40 -> 47 = 96 = 10010010
    stosb
    mov al, 0xcf	; flags + limit 48 -> 55 = cf = 1100 1111
    stosb
    mov al, 0 		; base 56 -> 64 = 0 
    stosb

    ; loading gdt
    lgdt [gdtrLimit]

    ; reading control register zero to switch to 32 bit protected mode
    ; by setting the first bit in this register and putting it
    ; back to its location
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; a far jump to flush the processor's pipeline
    ; and fix cs and ip (since we entered 32 bit mode)
    jmp 0x0008:0x9000

    ret