; ========================================
; function to discover if pci is supported
; or not and which config address
; mechanism is supported.
;
; args: none
; ret: none
; ========================================
func_DiscoverPCI:
    ;; preserving context
    pusha

    ;; int 0x1a ax 0x1b01 is used to collect pci info
    mov ax, 0xb101
    xor edi, edi
    int 0x1a

    ;; PCI is not installed if carry flag or ah is 0
    jc PCINotInstalled

    cmp ah, 0
    jne PCINotInstalled
    
    ;; store pci is supported
    push ax
    push bx
    xor ax, ax
    xor bx, bx
    mov bx, [BootHDRAddress]
    mov es, bx
    xor di, di
    mov di, 8
    mov al, 1
    stosb
    pop bx
    pop ax
    
    ;; is Config Address 1 supported?
    bt ax, 0
    jc PCIMech1

    ;; is Config Address 2 supported?
    bt ax, 1
    jc PCIMech2
    
    ;; do we need to check on special generation cycles?

PCIMech1:
    push ax
    push bx
    xor ax, ax
    xor bx, bx
    mov bx, [BootHDRAddress]
    mov es, bx
    xor di, di
    mov di, 9
    mov al, 1
    stosb
    pop bx
    pop ax
    jmp PCIEnd

PCIMech2:
    push ax
    push bx
    xor ax, ax
    xor bx, bx
    mov bx, [BootHDRAddress]
    mov es, bx
    xor di, di
    mov di, 9
    mov al, 2
    stosb
    pop bx
    pop ax
    jmp PCIEnd

PCINotInstalled:
    popa
    mov si, PCIErrorMsg
    call func_biosPrint
    jmp bootFailure

PCIEnd:
    ;; restoring context
    popa
    ret
