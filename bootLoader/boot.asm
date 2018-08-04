; second stage boot loader
BITS 16
jmp boot2
boot2Msg: db "Hello, World!", 0

global boot2
boot2:
	; clear screen
	xor ax, ax
	mov al, 02h
	int 10h

	; print boot2 welcoming message
    pusha
	push boot2Msg
    call printf
    popa
    jmp $


; ============= Functions ============= ;
; Function to print to screen
; args: address of first character in stack
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