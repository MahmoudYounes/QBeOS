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
    mov ax, 1111111111111111b 	; limit 0 -> 15 = ffff
    stosw
    mov ax, 0		            ; base 16 -> 31 = 0
    stosw
    mov al, 0		            ; base 32 -> 39 = 0
    stosb
    ; bit 0    (access) just set to 0
    ; bit 1    (rw)     for cs 0 means not readable 1 means readable
    ; bit 2    (DC)     for cs 1 means can be executed from Prvl level or lower. 0 only level in Prvl
    ; bit 3    (EX)     1 Code Segment, 0 Data Segment
    ; bit 4    (S)      1 for code or data segment 0 for system segment (eg task state segment)
    ; bit 5, 6 (Prvl)   define the Prvl level
    ; bit 7    (Pr)     present (must be 1 for all valid selectors)
    mov al, 11111110b
    stosb
    ; 48 -> 51 LIMIT, 52 -> 55 flags
    ; bit 4     0
    ; bit 5     0
    ; bit 6     (sz)    If 0 the selector defines 16 bit protected mode. If 1 it defines 32 bit protected mode
    ; bit 7     (Gr)    If 0 the limit is in 1 B blocks (byte granularity), if 1 the limit is in 4 KiB blocks
    mov al, 11001111b
    stosb
    mov al, 0 		            ; base 56 -> 64 = 0 
    stosb

    ; data descriptor (4 GB)
    mov ax, 1111111111111111b 	; limit 0 -> 15 = ffff
    stosw
    mov ax, 0		            ; base 16 -> 31 = 0
    stosw
    mov al, 0		            ; base 32 -> 39 = 0
    stosb
    ; bit 0    (access) just set to 0
    ; bit 1    (rw)     for ds 0 means not writable 1 means writable
    ; bit 2    (DC)     for ds 0 ds grows up 1 ds grows down (stack)
    ; bit 3    (EX)     1 Code Segment, 0 Data Segment
    ; bit 4    (S)      1 for code or data segment 0 for system segment (eg task state segment)
    ; bit 5, 6 (Prvl)   define the Prvl level
    ; bit 7    (Pr)     present (must be 1 for all valid selectors)
    mov al, 11110010b
    stosb
    ; 48 -> 51 LIMIT, 52 -> 55 flags
    ; bit 4     0
    ; bit 5     0
    ; bit 6     (sz)    If 0 the selector defines 16 bit protected mode. If 1 it defines 32 bit protected mode
    ; bit 7     (Gr)    If 0 the limit is in 1 B blocks (byte granularity), if 1 the limit is in 4 KiB blocks
    mov al, 11001111b
    stosb
    mov al, 0 		            ; base 56 -> 64 = 0 
    stosb

    ; loading gdt
    lgdt [gdtrLimit]

    ; setting up DS
    ; mov eax, 8h
    ; mov ds, eax 

    ; reading control register zero to switch to 32 bit protected mode
    ; by setting the first bit in this register and putting it
    ; back to its location
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; a far jump to flush the processor's pipeline
    ; and fix cs and ip (since we entered 32 bit mode)
    jmp 0x0008:0x9000
