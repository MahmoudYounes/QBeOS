; boot code to boot the kernel from iso9660 compliant CD
; boot sector size is 2kb ~ 2048 bytes
; writing the boot sector using EL torito specification
BITS 16
jmp _start
; boot loader data
BootDrive:					db 0
BootFailureMsg:				db "Booting sequence failed", 0
BootLoadingMsg:				db "loading...", 0

; kernel info
KernelName:					db "BOOT.BIN", 0x3b, 0x31 
KernelLBA:					dd 0
KernelLength:				dd 0
; DAP buffer
DAP:						db 10h			; DAP size (disk address packet)
							db 0			; unused
DAP2:						dw 1			; num sectors
DAP4:						dw 0			; offset for buffer
DAP6:						dw 0			; segment for buffer
DAP8:						dw 0			; absolute sector number highest
DAP10:						dw 0
DAP12:						dw 0 			; absolute sector number lowest
DAP14:						dw 0

global _start
_start:
	cli
	; preserving drive number and setting up registers for usage
	mov [BootDrive], dl
	
	; setting up the stack. stack grows downwards
	mov ax, 0
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
	xor ax, ax
	mov al, [BootDrive]
	push ax
	call func_ResetDisk
	popa

	; TODO: Check if int 13h extension methods 40h-48h exist and supported by BIOS. if not bootFailure


	; search for kernel file
	; start reading sectors from the drive
	; first 32kb (16 sectors) are empty start at sector 16
	pusha
	call func_readPrimaryVolumeDescriptor
	popa
	
	;pusha
	;call func_LocateKernelImage
	;popa

scanForBeOsImg:
	mov bx, 100h
	mov edx, [bx + 158]				; LBA of boot record
	pusha
	;H2H1 sector number highest 32 bits
	xor eax, eax
	mov eax, 0
	push eax
	;L2L1 sector number lowest 32 bits
	xor eax, eax
	mov eax, edx
	push eax

	; buffer segment
	xor ax, ax
	push ax

	; buffer offset
	xor ax, ax
	mov ax, 100h
	push ax 

	; number of sectors to read
	xor ax, ax
	mov ax, 1
	push ax
	
	mov ax, 0
	mov es, ax
	call func_ReadISOSector
	popa

	xor cx, cx
	mov bx, 100h			; reset buffer
loopLocateKernelFile:
	mov dl, [bx]			; size of entry in buffer
	mov cl, [ds:bx + 32]	; file identifier length
	mov si, bx
	add si, 33
	mov ax, ds
	mov es, ax
	mov di, KernelName
cmpStr:
	cmp cl, 0
	je fileFound
	dec cl
	cld
	cmpsb
	jne nextEntry
	jmp cmpStr
nextEntry:
	add bl, dl
	jmp loopLocateKernelFile

fileFound:
	mov eax, [bx + 2]
	mov [KernelLBA], eax
	mov eax, [bx + 10]
	mov [KernelLength], eax


loadKernel:

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

; Function that reads the primary volume descriptor from the cd
; args: none
; after the end of this function, the PVD will be at ds:100h in memory
func_readPrimaryVolumeDescriptor:
	mov edx, 0
	mov ecx, 10h
rpvd_read:
	pusha
	;H2H1 sector number highest 32 bits
	xor eax, eax
	mov eax, edx
	push eax
	;L2L1 sector number lowest 32 bits
	xor eax, eax
	mov eax, ecx		
	push eax

	; buffer segment
	xor ax, ax
	push ax

	; buffer offset
	xor ax, ax
	mov ax, 100h
	push ax 

	; number of sectors to read
	xor ax, ax
	mov ax, 1
	push ax
	
	call func_ReadISOSector
	popa

	mov bx, 100h
	mov al, [ds:bx]
	cmp al, 1
	je rpvd_ret
	inc ecx
	jmp rpvd_read
rpvd_ret:
	ret

; function to locate the kernel Image
; args: none
; after this function completes the LBA that contains the file will be in KernelLBA and size will be in KernelLength
;func_LocateKernelImage