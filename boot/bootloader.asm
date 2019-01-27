;===================================================================
;				        BeOS BootLoader Code
;				        ====================
;
; boot code to boot the kernel from iso 9660 compliant CD
; works on 8086 architecture, 32 bits, single CPU 
; boot sector size is 2kb ~ 2048 bytes
;===================================================================

; TODO: make more explicit error messages
; TODO: discover memory layout

[BITS 16]
[ORG 0x0000]
jmp _start

global _start
_start:
	; setting up segments and offsets
	cli
	mov ax,0x07c0							
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax

	xor ax,ax
	mov ss,ax
	mov sp,0xffff	
	sti

	; far jmp to make sure that cs and ip are the correct values
	; 5 = 1 opcode + 2 segment + 2 offset
	jmp 0x07c0:$ + 5
	
	; preserving boot drive number
	mov BYTE[BootDrive], dl

	; print loading msg
	mov si, BootLoadingMsg
	call func_biosPrintf

	; resetting boot drive
	mov dl, [BootDrive]
	call func_ResetDisk

	; TODO: Check if int 13h extension methods 40h-48h exist and supported by BIOS. if not bootFailure
	;

	; reading primary volume descriptor to locate the kernel files
	; first 32kb (16 sectors) are empty start at sector 16
	mov eax, 100h
	mov es, eax
	xor edi, edi
	call func_readPrimaryVolumeDescriptor

	; search for kernel file	
	mov eax, 100h
	mov es, eax
	xor edi, edi
	call func_LocateKernelImage

	call func_LoadKernel

	call func_EnableA20

	call func_enableProtectedModeAndJmpKernel

	cli
	hlt
bootFailure:
	mov si, BootFailureMsg
	call func_biosPrintf
bootloaderEnd:
	cli
	hlt

%include "./screen.asm"
%include "./isoUtilities.asm"
%include "./kernelLoad.asm"
%include "./enableA20.asm"
%include "./protectedMode.asm"


; boot loader data
BootDrive:					db 0
BootFailureMsg:				db "Booting sequence failed", 0
BootLoadingMsg:				db "loading BeOS...", 0
BytesPerSector:				dw 0

; kernel info
KernelName:					db "KERNEL.BIN", 0x3b, 0x31 
KernelLBA:					dd 0
KernelLength:				dd 0

; gdtr
gdtrLimit:	 				dw 0    		; limit (size)
gdtrBaseLow: 				dw 0    		; base  (address in memory)
gdtrBaseHigh: 				dw 0    		; base  (address in memory)

; DAP buffer
DAP:						db 10h			; DAP size (disk address packet)
							db 0			; unused
dapNumSectors:				dw 0			; num sectors
dapBufferOffset:			dw 0			; offset for buffer
dapBufferSegment:			dw 0			; segment for buffer
dapSectorNumL:				dd 0			; absolute sector number low
dapSectorNumH:				dd 0 			; absolute sector number high

; screen state
currentCursorPosition:		db 0			; the current position of the cursor state
whiteOnBlackConst:			equ 0x0f		; const added to video memory
rowsLimit:					equ 80
colsLimit:					equ 25

TIMES 2046-($-$$) db 0
dw 0xaa55