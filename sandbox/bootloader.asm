BITS 16	; assembl code for real mode
ORG 0x0 ; organize code @ location 0
; make sure the kernel resides at address 1mb (using linker)
; activate A20 and switch to unreal mode.
cli

xor ax, ax ; ax = 0
mov es, ax

not ax ; ax = 0xFFFF
mov ds, ax

mov di, 0x0500
mov si, 0x0510

mov al, byte [es:di]
push ax

mov al, byte [ds:si]
push ax

mov byte [es:di], 0x00
mov byte [ds:si], 0xFF

cmp byte [es:di], 0xFF

pop ax
mov byte [ds:si], al

pop ax
mov byte [es:di], al

jne enable_a20

mov ax, 1

jmp continue

enable_a20:


continue:

; define how to jump to c code here
jmp 0x1000:0x0
; fill in the rest of the bytes untill byte 510 with zeros

; fill bytes 510 and 511 (the last 2 bytes) with 0x55AA respectively

