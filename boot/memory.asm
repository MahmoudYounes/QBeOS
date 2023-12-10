; ========================================
; function to discover memory layout
;
; args: none
; ========================================
func_DiscoverMemory:
    pusha

    ; setting up buffer
    mov ax, [MLTBufAddress]
    mov es, ax
    xor di, di
    xor ebx, ebx
NextEntry:
    mov edx, 0x534D4150
    mov eax, 0xE820
    mov ecx, 0x18               ; 24
    int 15h
    jc MemDiscoveryErr
    cmp bx, 0
    je MemDiscoveryEnd

    add di, cx
    mov eax, 0xE820
    mov ecx, 0x18
    jmp NextEntry

MemDiscoveryErr:
    popa
    mov si, MemDiscoveryFailureMsg
    call func_biosPrint
    jmp bootFailure

MemDiscoveryEnd:
    popa
    ret
