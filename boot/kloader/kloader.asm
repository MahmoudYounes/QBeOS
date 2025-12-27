;===================================================================
;				        QBeOS KernelLoader Code
;				        =======================
;
; boot code to load the QBeOS kernel.
; works on 8086 architecture.
; this is the second stage bootloader and is 8192 bytes.
; we should eventually prepare the environment for the kernel
; to run on.
; list of arguments we need:
; - boot drive number. passed on the stack.
;===================================================================
[BITS 16]
[ORG 0x8400]
jmp _loadkernel
global _loadkernel
_loadkernel:
  ; clear the state
  mov [BootDrive], dl
  cli
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax  

  ; reset the stack. make no assumptions whatsoever about previous state.
	mov ax, 0x02d0
	mov ss, ax
	xor ax, ax
	mov sp, 0x4eff 

  ; set interrupts to use BIOS
  sti
  
  ; far jmp to make sure that cs and ip are the correct values
  ; 5 = 1 opcode + 2 segment + 2 offset
	jmp 0h: $ + 5

  
  call func_biosClearScreen

  mov si, LoaderHelloMsg
  call func_biosPrint
    
	call func_EnableA20

	call func_DiscoverMemory

  call func_DiscoverPCI

	call func_PrepareGDT

  ;mov si, HaltingMessage
  ;call func_biosPrint
  
  ;jmp bootloaderEnd
  
  call func_LoadKernelLoader

	call func_EnableProtectedModeAndJmpKernel

bootFailure:
	mov si, BootFailureMsg
	call func_biosPrint
  jmp bootloaderEnd
bootloaderEnd:
	cli
	hlt  
  
  ;jmp $

;  call func_EnableA20

%include "../fs/iso9660Loader.asm"
%include "../drivers/screen/screen.asm"
%include "../drivers/disk/disk.asm"
%include "./memory.asm"
%include "./enableA20.asm"
%include "./gdt.asm"
%include "./pci.asm"
%include "./protectedMode.asm"

BootDrive: db 0
LoaderFailuresMsg: db "Booting sequence failed in second stage", 0
LoaderHelloMsg: db "Loading QBeOS...", 0
HaltingMessage: db "Finished pre-kernel setup... hlting...", 0
MemDiscoveryFailureMsg:	    db "Memory discover failed", 0
PCIErrorMsg:                db "Couldn't discover PCI", 0

;; Screen State
currentCursorPosition:		db 0; the current position of the cursor state
whiteOnBlackConst:			  equ 0x0f; const added to video memory
rowsLimit:					      equ 80
colsLimit:					      equ 25

BootHDRAddress:    dw 0x7800
; Bootloader buffer pointer
BLBufPointer:	dw 0x00d0

; Memory Layout Table Buf
MLTBufAddress:     dw 0x7000
MemRegionsCount:   dd 0

;; DAP buffer
DAP:						      db 10h; DAP size(disk address packet)
							        db 0; unused
dapNumSectors:				dw 0; num sectors
dapBufferOffset:			dw 0; offset for buffer
dapBufferSegment:			dw 0; segment for buffer
dapSectorNumL:				dd 0; absolute sector number low
dapSectorNumH:				dd 0; absolute sector number high

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

BootFailureMsg:				      db "fail", 0

;times 8192 - ($ - $$) db 0 

