;===================================================================
;				        BeOS BootLoader Code
;				        ====================
;
; boot code to boot the kernel from iso 9660 compliant CD
; works on 8086 architecture, 32 bits, single CPU
; boot sector size is 2kb ~2048 bytes
;===================================================================

; TODO: make more explicit error messages
; TODO: discover memory layout

[BITS 16]
[ORG 0x7C00]
jmp _start

global _start
_start:
; setting up segments and offsets
cli
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
    ;;  setup stack
	mov ax, 0x02d0
	mov ss, ax
	xor ax, ax
	mov sp, 0x4eff; for reference to why this value check docs / memory_layout.md or docs / OSMap.txt
    ; 0x02d0 * 16 + 0x4eff = 0x7bff
sti

    ; far jmp to make sure that cs and ip are the correct values
    ; 5 = 1 opcode + 2 segment + 2 offset
	jmp 0h: $ + 5

    ; preserving boot drive number
	mov BYTE[BootDrive], dl

    ; print loading msg
	mov si, BootLoadingMsg
	call func_biosPrint

	call func_EnableA20

    ; resetting boot drive
	mov dl, [BootDrive]
	call func_ResetDisk

    ; TODO: Check if int 13h extension methods 40h - 48h exist and supported by BIOS.if not bootFailure
    ;
	; reading primary volume descriptor to locate the kernel files
    ; first 32kb(16 sectors) are empty start at sector 16
    ; the es contains the begining of the buffer to load into
	mov eax, [PVDBufAddress]
	mov es, eax
	xor edi, edi
	call func_ReadPrimaryVolumeDescriptor

	mov eax, [PVDBufAddress]
	mov es, eax
	xor edi, edi
	call func_LocateKernelImage

	call func_LoadKernel

	call func_DiscoverMemory

  call func_DiscoverPCI

  ; mark header as complete
  xor bx, bx
  xor cx, cx
  mov bx,0x05be
  mov ecx, BootHDRAddress
  ;mov byte [ecx + 11], bl
  ;mov byte [ecx + 12], bh

	call func_PrepareGDT

	call func_EnableProtectedModeAndJmpKernel

    cli
    hlt
bootFailure:
	mov si, BootFailureMsg
	call func_biosPrint
bootloaderEnd:
	cli
	hlt

;; includes sorted please!
%include "./screen.asm"
%include "./enableA20.asm"
%include "./isoUtilities.asm"
%include "./kernelLoad.asm"
%include "./memory.asm"
%include "./pci.asm"
%include "./gdt.asm"
%include "./protectedMode.asm"

;; boot loader data
BootDrive:					        db 0
BootFailureMsg:				      db "Booting sequence failed", 0
MemDiscoveryFailureMsg:	    db "Memory discover failed", 0
BootLoadingMsg:				      db "loading QBeOS...", 0
PCIErrorMsg:                db "Couldn't discover PCI", 0
BytesPerSector:				      dw 0

    ; kernel info
KernelName:					db "KERNEL.IMG", 0x3b, 0x31
KernelLBA:					dd 0
KernelLength:				dd 0

    ; PVD begining address
PVDBufAddress:	dw 0x0050

    ; Bootloader buffer pointer
BLBufPointer:	dw 0x00d0

    ; Memory Layout Table Buf
MLTBufAddress:     dw 0x7000
MemRegionsCount:   dd 0
BootHDRAddress:    dw 0x7800

    ; gdtr
gdtData:
	dd 0x0000
	dd 0x0000
    ;;Code Descriptor
	dw 0xFFFF; Limit(Low)
	dw 0x0000; Base(Low)
	DB 0x00; Base(Middle)
	DB 10011010b; Access
	DB 11001111b; Granularity
	DB 0x00; Base(High)
    ;;Data Descriptor
	dw 0xFFFF; Limit(Low)
	dw 0x0000; Base(Low)
	DB 0x00; Base(Middle)
	DB 10010010b; Access
	DB 11001111b; Granularity
	DB 0x00; Base(High)
gdtEnd:
GDT:
	dw gdtEnd - gdtData - 1;GDT Size - 1
	dd gdtData;Base Of GDT


;; DAP buffer
DAP:						      db 10h; DAP size(disk address packet)
							        db 0; unused
dapNumSectors:				dw 0; num sectors
dapBufferOffset:			dw 0; offset for buffer
dapBufferSegment:			dw 0; segment for buffer
dapSectorNumL:				dd 0; absolute sector number low
dapSectorNumH:				dd 0; absolute sector number high

;; screen state
currentCursorPosition:		db 0; the current position of the cursor state
whiteOnBlackConst:			  equ 0x0f; const added to video memory
rowsLimit:					      equ 80
colsLimit:					      equ 25

;;; Check if bootloader is bigger than 512 bytes emit error
%if 2046 - ($ - $$) < 0
    %error "bootloader exceeded 2046 bytes"
%endif

;;; fill in the rest of the output file for isofs compatability
TIMES 2046 - ($ - $$) db 0
dw 0xaa55
