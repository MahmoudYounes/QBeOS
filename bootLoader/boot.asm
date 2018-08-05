; second stage boot loader
BITS 16
jmp boot2
boot2Msg: db "Loading stage two...", 0
gdtrContent: dw 0    ; limit (size)
			 dd 0    ; base  (address in memory)

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
	
	; TODO: search for the kernel and load it

	; TODO: enable the A20 line



	cli
	; assigning right address to gdt
	mov [gdtrContent], word 0xffff		; 4 GB size
	mov [gdtrContent + 1], word 0x0000	; base address
	mov [gdtrContent + 2], word 0x0800	; base address

	; creating 3 entries in the GDT for now
	; null descriptor
	mov ax, 0
	mov es, ax
	mov di, 0x0800
	mov cx, 4
	rep stosw

	; code descriptor (4 GB)
	mov ax, 0xffff 	; limit 0 -> 15 = ffff
	stosw
	mov ax, 0		; base 16 -> 31 = 0
	stosw
	mov al, 0		; base 32 -> 39 = 0
	stosb
	mov al, 0x9a	; access byte 40 -> 47 = 9a = 10011010
	stosb
	mov al, 0xcf	; flags + limit 48 -> 55 = cf = 1100 1111
	stosb
	mov al, 0 		; base 56 -> 64 = 0 
	stosb

	; data descriptor (4 GB)
	mov ax, 0xffff 	; limit 0 -> 15 = ffff
	stosw
	mov ax, 0		; base 16 -> 31 = 0
	stosw
	mov al, 0		; base 32 -> 39 = 0
	stosb
	mov al, 0x92	; access byte 40 -> 47 = 9a = 10010010
	stosb
	mov al, 0xcf	; flags + limit 48 -> 55 = cf = 1100 1111
	stosb
	mov al, 0 		; base 56 -> 64 = 0 
	stosb

	; loading gdt
	lgdt gdtrContent

	; TODO: loading idt

	; TODO: enter protected mode 

	; TODO: define the jmp to the kernel

	; for now hlt the processor
	cli
	hlt
	



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