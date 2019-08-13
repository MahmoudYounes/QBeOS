func_EnableProtectedModeAndJmpKernel:
    ; setting up DS
    ; when in protected mode the bits meanings in the segmant registers differ
    ; bits 0, 1 -> describes prvl
    ; bit  2    -> table indicator (0 : GDT, 1 : LDT)
    ; bits 3>15 -> descriptor selector. hence 8h -> (descriptor number 1:00001)(ti:0)(prvl:00)
    ; remember null descriptor is at 0
    mov eax, 10h
    mov ds, eax

    ; reading control register zero to switch to 32 bit protected mode
    ; by setting the first bit in this register and putting it
    ; back to its location
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; a far jump to flush the processor's pipeline
    ; and fix cs and ip (since we entered 32 bit mode)
    jmp 0x0008:0x9000
