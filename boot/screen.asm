; ========================================
; Function to print to screen using BIOS
; args: 
; si: address of first character in stack
; ========================================
func_biosPrintf:
	pushad

loop_biosPrintf:		; printing loop
	lodsb
	cmp al, 0
	je end_biosPrintf
	mov ah, 0x0e
	int 0x10
	jmp loop_biosPrintf
end_biosPrintf:
	popad
	ret

; ========================================
; Function to clear screen using BIOS
; args: 
; si: address of first character in stack
; ========================================
func_biosClearScreen:
	xor ax, ax
	mov al, 02h
	int 10h
    ret

; ========================================
; Function to print to screen directly 
; using video ram (useful in 32 bit
; mode)
; args: 
; si: address of first character in stack
; ========================================
func_printf:
	pushad
	mov bx, 0xb800
	mov es, bx

loop_printf:
	lodsb
	cmp al, 0
	je end_printf
	mov ah, whiteOnBlackConst
	mov bx, [currentCursorPosition]
	mov [es:bx], ax
	inc bx
	inc bx
	mov [currentCursorPosition], bx
	jmp loop_printf
end_printf:
	popad
	ret

; TODO: implement clear screen function
