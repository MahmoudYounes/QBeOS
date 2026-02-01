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
; - boot drive number. passed in dl.
;===================================================================
[ORG 0x8400]
[BITS 16]
jmp _start 
_start:
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
  
  ; Note: Before we can go into the C code execution we need to prepare the
  ; 32 bit enviornment

  call func_biosClearScreen

  mov si, LoaderHelloMsg
  call func_biosPrint
    
	call func_EnableA20

	call func_DiscoverMemory

  call func_DiscoverPCI
  
  xor eax, eax
  xor ebx, ebx
  ; we move the location in memory we will load the second stage BL into
  mov ax, [QBekLoadAddr]
	mov es, eax
	xor edi, edi
  ; start with the first sector. sector 0 is the MBR
  mov bl, [QBekSectorStart]
  mov cl, 0
loadStage3:
	call func_ReadLBA
  inc bl
  inc cl
  add edi, [QBekSectorSize]
  cmp cl, [QBekSectorsCount]
  jle loadStage3
 
  ; TODO: Mark the boot header here

  call func_PrepareGDT

  ; reading control register zero to switch to 32 bit protected mode
  ; by setting the first bit in this register and putting it
  ; back to its location
  mov eax, cr0
  or eax, 0x1
  mov cr0, eax

  ; setting up DS
  ; when in protected mode the bits meanings in the segmant registers differ
  ; bits 0, 1 -> describes prvl
  ; bit  2    -> table indicator (0 : GDT, 1 : LDT)
  ; bits 3>15 -> descriptor selector. hence code descriptor 8h -> (descriptor number 1:00001)(ti:0)(prvl:00)
  ; remember null descriptor is at 0
  mov eax, 0x10
  mov ds, eax
  mov fs, eax
  mov gs, eax
  mov ss, eax
  mov es, eax
  mov esp, 0x3fffff
  jmp 0x08:0x8800

bootFailure:
	mov si, LoaderFailuresMsg
	call func_biosPrint
  jmp bootloaderEnd

bootloaderEnd:
	cli
	hlt  
  
 %include "./screen.asm"
 %include "./disk.asm"
 %include "./memory.asm"
 %include "./enableA20.asm"
 %include "./gdt.asm"
 %include "./pci.asm"

;;; MSGS
LoaderFailuresMsg:          db "Booting sequence failed in second stage", 0
LoaderHelloMsg:             db "Loading QBeOS...", 0
HaltingMessage:             db "Finished pre-kernel setup... hlting...", 0
MemDiscoveryFailureMsg:	    db "Memory discover failed", 0
PCIErrorMsg:                db "Couldn't discover PCI", 0

;;; QBek specific data
BootDrive:        db 0
QBekLoadAddr:     dw 0x880
QBekSectorStart:  db 4
QBekSectorsCount: db 62
QBekSectorSize:   dd 512

;;; BOOTHDR
BootHDRAddress:             dw 0x7800

;;; Memory Layout Table Buf
MLTBufAddress:              dw 0x7000
MemRegionsCount:            dd 0

;;; DAP buffer
DAP:						      db 10h; DAP size(disk address packet)
							        db 0; unused
dapNumSectors:				dw 0; num sectors
dapBufferOffset:			dw 0; offset for buffer
dapBufferSegment:			dw 0; segment for buffer
dapSectorNumL:				dd 0; absolute sector number low
dapSectorNumH:				dd 0; absolute sector number high

; gdtr
gdtData:
;;Null descriptor
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

