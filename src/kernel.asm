BITS 32
kmainMsg: db "Hello, World!", 0x0
kmain:
pusha
push kmainMsg
call printf
popa
hlt


; Function to print to screen
; args: address of first character
printf:				; function preparation
	pop bp 			; poping old ip in bp
	pop si 			; poping arguments
	push bp			; re-adding the old ip
loop_printf:		; printing loop
	lodsb
	cmp al, 0
	je endPrintf
	mov ah, 0x0e
	int 0x10
	jmp loop_printf
endPrintf:
	ret