; ========================================
; Function to print to screen using BIOS
; calls.
;
; args: 
; si: address of first character in stack
; ========================================
func_biosPrint:
	pushad

loop_biosPrint:		; printing loop
	lodsb
	cmp al, 0
	je end_biosPrint
	mov ah, 0x0e
	int 0x10
	jmp loop_biosPrint
end_biosPrint:
	popad
	ret

; ========================================
; Function to clear screen using BIOS
; ========================================
func_biosClearScreen:
	xor ax, ax
	mov al, 02h
	int 10h
  ret

