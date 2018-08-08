; boot code to boot the kernel from iso9660 compliant CD
; boot sector size is 2kb ~ 2048 bytes
; think of this code as code added on cd
; writing the boot sector using EL torito specification
BITS 16
; BootRecordFlag: 			db 0
; BootRecordId: 				db "CD001"
; VolumeDescriptorVersion: 	db 0x0001
; BootSystemId:				db "EL TORITO SPECIFICATION000000000"
; times 70 - 39 				db 0		
jmp _start

BootDrive:			db 0								; 1     this is the drive number
bootFailureMsg:		db "Booting sequence failed", 0
bootLoadingMsg:		db "loading...", 0

global _start
_start:
	cli
	; preserving drive number and setting up registers for usage
	mov [BootDrive], dl
	xor ax, ax
	mov ds, ax
	mov es, ax
	
	; setting up the stack. stack grows downwards
	mov ss, ax
	mov sp, 0x7C00
	sti

	; print loading msg
	pusha
	push bootLoadingMsg
	call printf
	popa

	; loading second stage bootloader
	pusha
	mov ax, 2
	push ax
	call LoadSecondStage
bootFailure:
	pusha
	push bootFailureMsg
	call printf
	popa
bootloaderEnd:
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

; Function to find second stage and load into it. default 3 retries and fail boot sequence afterwards
; args: the number of sector to read in lba mode
LoadSecondStage:
	mov si, ax 		; preserving lba in si
	mov di, 0
loopReadDiskSector:
	; reseting floppy drive
	mov dl, [BootDrive]
	xor ax, ax
	int 13h
	jc bootFailure
	
	; buffer to load second stage bootloader
	mov ax, 0x0000
	mov es, ax
	mov bx, 0x9000

	mov ah, 0x02	; read function in int 13h
	mov al, 0x03	; number of sectors to read
	mov ch, 0x00	; cylinder to read
	mov cl, 0x02	; sector to read
	mov dh, 0x00	; head to read
	mov dl, [BootDrive] ; boot drive number
	int 13h
	jc loopReadDiskSector
	mov ax, 0x9000
	mov es, ax
	xor bx, bx
	jmp 0x9000
	

times 510 - ($ - $$) db 0
dw 0xaa55

