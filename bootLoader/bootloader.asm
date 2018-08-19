; boot code to boot the kernel from iso9660 compliant CD
; boot sector size is 2kb ~ 2048 bytes
; think of this code as code added on cd
; writing the boot sector using EL torito specification
BITS 16
jmp _start

PrimaryVolumeDescriptor: 	dd 1
BootFileLocation:			dd 1
BootFileLength:				dd 1
Checksum:					dd 1
Reserved:					db 40
BootDrive:					db 0
BootFailureMsg:				db "Booting sequence failed", 0
BootLoadingMsg:				db "loading...", 0
KernelName:					db "BeOs.img", 0
DAP:						db 10h			; DAP size (disk address packet)
							db 0			; unused
DAP2:						dw 1			; num sectors
DAP4:						dw 0			; offset for buffer
DAP6:						dw 0			; segment for buffer
DAP8:						dw 0			; absolute number highest
DAP10:						dw 0
DAP12:						dw 0 			; absolute number lowest
DAP14:						dw 0


global _start
_start:
	cli
	; preserving drive number and setting up registers for usage
	mov [BootDrive], dl
	
	; setting up the stack. stack grows downwards
	xor ax, ax
	mov ss, ax
	mov sp, 0x7C00
	sti

	; print loading msg
	pusha
	push BootLoadingMsg
	call func_printf
	popa

	; resetting boot drive
	pusha
	mov ax, [BootDrive]
	push ax
	call func_ResetDisk
	popa

	; search for kernel file
	; read the volume descriptor
	pusha
	;H2H1 sector number highest 32 bits
	xor eax, eax
	push eax
	;L2L1 sector number lowest 32 bits
	xor eax, eax
	mov eax, 10h
	push eax

	; buffer segment
	xor ax, ax
	push ax

	; buffer offset
	xor ax, ax
	mov ax, 1000h
	push ax 

	; number of sectors to read
	xor ax, ax
	mov ax, 1
	push ax
	
	call func_ReadISOSector
	popa
	




	; loading second stage bootloader
	; pusha
	; mov ax, 2
	; push ax
	; call LoadSecondStage
bootFailure:
	pusha
	push BootFailureMsg
	call func_printf
	popa
bootloaderEnd:
	cli
	hlt

; ============= Functions ============= ;
; Function to print to screen
; args: address of first character in stack
func_printf:				; function preparation
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


; Reset the drive passed in as argument
; args: drive number as byte in stack
func_ResetDisk:
	pop bp
	pop dx
	push bp
	mov cx, 3
rd_loop_trials:
	mov ah, 0
	int 13h
	jc rd_ResetFail
	ret
rd_ResetFail:
	dec cx
	cmp cx, 0
	je bootFailure
	jmp rd_loop_trials

; Read Sector from iso image
; args: drive number(2 byte), number of sectors to read (2 bytes), offset of buffer (2 bytes), segment of buffer (2 bytes)
; absolute number of start of the sectors to read (8 bytes)
func_ReadISOSector:
	pop bp		; ip
	pop ax		; number of sectors to read
	pop bx		; offset of read data buffer
	pop dx 		; segment of read data buffer
	pop ecx		; start sector absolute number
	pop edi
	push bp		; ip

	; filling DAP
	mov [DAP2], ax
	mov [DAP4], bx
	mov [DAP6], dx
	mov [DAP8], cx
	shr ecx, 16
	mov [DAP10], cx
	shr ecx, 16
	mov [DAP12], di
	shr edi, 16
	mov [DAP14], di

	mov cx, 3
risos_loop_trials:
	xor ax, ax
	mov dx, ax
	mov ds, ax
	mov si, DAP
	mov dl, [BootDrive]
	xor ax, ax
	mov ah, 42h
	int 13h
	jc risos_fail
	ret
risos_fail:
	dec cx
	cmp cx, 0
	je bootFailure
	jmp risos_loop_trials



; Function to find second stage and load into it. default 3 retries and fail boot sequence afterwards
; args: the number of sector to read in lba mode
; LoadSecondStage:
; 	mov si, ax 		; preserving lba in si
; 	mov di, 0
; loopReadDiskSector:
; 	; reseting floppy drive
; 	mov dl, [BootDrive]
; 	xor ax, ax
; 	int 13h
; 	jc bootFailure
	
; 	; buffer to load second stage bootloader
; 	mov ax, 0x0000
; 	mov es, ax
; 	mov bx, 0x9000

; 	mov ah, 0x02	; read function in int 13h
; 	mov al, 0x03	; number of sectors to read
; 	mov ch, 0x00	; cylinder to read
; 	mov cl, 0x02	; sector to read
; 	mov dh, 0x00	; head to read
; 	mov dl, [BootDrive] ; boot drive number
; 	int 13h
; 	jc loopReadDiskSector
; 	mov ax, 0x9000
; 	mov es, ax
; 	xor bx, bx
; 	jmp 0x9000
	

