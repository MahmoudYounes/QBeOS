BITS 32
jmp kmain
kmainMsg: db "Hello, World!", 0x0
global kmain

kmain:

	pusha
	call clearScreen
	push kmainMsg
	call printf
	popa
	hlt


; Function to clear screen
clearScreen:
	xor ax, ax
	mov al, 02h
	int 10h
	ret

; Function to print to screen
; args: address of first character
printf:					; function preparation
	pop ebp 			; poping old ip in bp
	pop si 				; poping arguments
	push ebp			; re-adding the old ip
loop_printf:			; printing loop
	lodsb
	cmp al, 0
	je endPrintf
	mov ah, 0x0e
	int 0x10
	jmp loop_printf
endPrintf:
	ret