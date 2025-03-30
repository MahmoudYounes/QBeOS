; ========================================
; function to discover memory layout
; the resulting memory tables structured
; like the following:
; - 4 bytes: memory regions table length
; - c * 20 byte entries
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
    mov eax, [MemRegionsCount]
    inc eax
    mov [MemRegionsCount], eax

    mov edx, 0x534D4150
    mov eax, 0xE820
    mov ecx, 0x18               ; 24
    int 15h
    jc MemDiscoveryErr
    cmp bx, 0
    je MemDiscoveryEnd

    add di, cx
    jmp NextEntry

MemDiscoveryErr:
    popa
    mov si, MemDiscoveryFailureMsg
    call func_biosPrint
    jmp bootFailure

MemDiscoveryEnd:
    ;; store information in boot HDR
    mov ax, [BootHDRAddress]
    mov es, ax
    xor di, di
    mov eax, [MemRegionsCount]
    stosd

    xor eax, eax
    mov ax, [MLTBufAddress]
    shl eax, 4
    stosd
       
    popa
    ret
