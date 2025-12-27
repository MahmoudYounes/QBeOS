;===================================================================
;				        QBeOS BootLoader Code
;				        =====================
;
; boot code to boot the mahcine and load the kernel loader from 
; FAT32 mediums 
; works on 8086 architecture, 32 bits, single CPU.
; boot sector size is 512b.
; the kernel loader must exist in the first partition.
; the kernel itself can exist in any partition.
;===================================================================

[BITS 16]
[ORG 0x7C00]
jmp _start ; this jump will be handled by the tool that will write the MBR
times 88 db 0 ; a tool will use this area to write require DS 
global _start
_start:
  cli
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
  ;;  setup stack
	mov ax, 0x02d0
	mov ss, ax
	mov sp, 0x4eff 
  ; for reference to why this value check docs / memory_layout.md or docs / OSMap.txt
  ; 0x02d0 * 16 + 0x4eff = 0x7bff

  sti

  ; far jmp to make sure that cs and ip are the correct values
  ; 5 = 1 opcode + 2 segment + 2 offset
	jmp 0h: $ + 5

  ; preserving boot drive number
	mov BYTE[BootDrive], dl

  ; resetting boot drive
	mov cx, 3
rd_loop_trials:
	mov ah, 0
	int 13h
	jc rd_ResetFail
  jmp rd_ResetDone
rd_ResetFail:
	dec cx
	cmp cx, 0
	je bootFailure
	jmp rd_loop_trials
rd_ResetDone:

  ; Check if int 13h extension methods 40h - 48h exist and supported by BIOS.
  ; if not bootFailure
  mov ah, 0x41
  mov dl, [BootDrive]
  mov bx, 0x55aa
  int 13h
  jc bootFailure 

  ; the second stage bootloader exists in the 4 sectors after the first sector
  ; the second stage bootloader is fixated to 2048 bytes
  xor eax, eax
  ; ebx: counter to the number of LBAs loaded and which LBA we will load 
  xor ebx, ebx
  ; we move the location in memory we will load the second stage BL into
  mov ax, [MBRBufAddress]
	mov es, eax
	xor edi, edi
  ; start with the first sector. sector 0 is the MBR
  mov ebx, 1   
loadStage2:
	call func_ReadLBA
  inc ebx
  add edi, 512
  cmp ebx, 4
  jle loadStage2 


; important TODO: do we need the boot drive here? 
jumpStage2:
  mov dl, [BootDrive]
  jmp 0h:0x8400 

bootFailure:
	mov si, BootFailureMsg
	call func_biosPrint
  jmp bootloaderEnd
bootloaderEnd:
	cli
	hlt

; ========================================
; Function that reads a sector from disk
; using LBA
;
; args:
; ebx: sector LBA
; es : segment to load the lba in
; di : offset to load the lba in
;
; after the end of this function, the LVA will be at es:di in memory
; ========================================
func_ReadLBA:
  pushad

	mov cx, 1
  mov ax, es
	mov WORD[dapNumSectors], cx
	mov WORD[dapBufferOffset], di
	mov WORD[dapBufferSegment], ax
	mov DWORD[dapSectorNumL], ebx
	mov DWORD[dapSectorNumH], 0	

	mov ah, 0x42
	mov dl, BYTE[BootDrive]
	mov si, DAP
	int 13h
	jc bootFailure
  popad
  ret

func_biosPrint:
loop_biosPrint:		; printing loop
	lodsb
	cmp al, 0
	je end_biosPrint
	mov ah, 0x0e
	int 0x10
	jmp loop_biosPrint
end_biosPrint:
  ret

;%include "./enableA20.asm"
;%include "../drivers/disk/disk.asm"
;%include "./kernelLoader.asm"
;%include "./memory.asm"
;%include "./pci.asm"
;%include "./gdt.asm"
;%include "./protectedMode.asm"

;; boot loader data
BootDrive:					        db 0

BootFailureMsg:				      db "fail", 0
;MemDiscoveryFailureMsg:	    db "Memory discover failed", 0
BootLoadingMsg:				      db "laod", 0
;PCIErrorMsg:                db "Couldn't discover PCI", 0
;BytesPerSector:				      dw 0

; kernel loader info
KernelLoaderName:					db "KLOADER.BIN", 0x3b, 0x31
KernelLoaderLBA:					dd 0
KernelLoaderLength:				dd 0

; kernel load address
MBRBufAddress:	dw 0x0840

; Memory Layout Table Buf
;MLTBufAddress:     dw 0x7000
;MemRegionsCount:   dd 0
;BootHDRAddress:    dw 0x7800

; gdtr
;gdtData:
;	dd 0x0000
;	dd 0x0000
    ;;Code Descriptor
;	dw 0xFFFF; Limit(Low)
;	dw 0x0000; Base(Low)
;	DB 0x00; Base(Middle)
;	DB 10011010b; Access
;	DB 11001111b; Granularity
;	DB 0x00; Base(High)
    ;;Data Descriptor
;	dw 0xFFFF; Limit(Low)
;	dw 0x0000; Base(Low)
;	DB 0x00; Base(Middle)
;	DB 10010010b; Access
;	DB 11001111b; Granularity
;	DB 0x00; Base(High)
;gdtEnd:
;GDT:
;	dw gdtEnd - gdtData - 1;GDT Size - 1
;	dd gdtData;Base Of GDT


;; DAP buffer
DAP:						      db 10h; DAP size(disk address packet)
							        db 0; unused
dapNumSectors:				dw 0; num sectors
dapBufferOffset:			dw 0; offset for buffer
dapBufferSegment:			dw 0; segment for buffer
dapSectorNumL:				dd 0; absolute sector number low
dapSectorNumH:				dd 0; absolute sector number high

; fill in the rest of the output file for isofs compatability
;TIMES 446 - ($ - $$) db 0

; MBR Data
PartBegin:
Part1:                
P1DriveAttr:            db 0x80
P1CPartStart:           db 0
P1HPartStart:           db 0
P1SPartStart:           db 0
P1PartType:             db 0
P1CPartEnd:             db 0
P1HPartEnd:             db 0
P1SPartEnd:             db 0
P1LBAStart:             dd 0
P1LBACount:             dd 0

Part2:                
P2DriveAttr:            db 0x0
P2CPartStart:           db 0
P2HPartStart:           db 0
P2SPartStart:           db 0
P2PartType:             db 0
P2CPartEnd:             db 0
P2HPartEnd:             db 0
P2SPartEnd:             db 0
P2LBAStart:             dd 0
P2LBACount:             dd 0

Part3:                
P3DriveAttr:            db 0x0
P3CPartStart:           db 0
P3HPartStart:           db 0
P3SPartStart:           db 0
P3PartType:             db 0
P3CPartEnd:             db 0
P3HPartEnd:             db 0
P3SPartEnd:             db 0
P3LBAStart:             dd 0
P3LBACount:             dd 0

Part4:                
P4DriveAttr:            db 0x0
P4CPartStart:           db 0
P4HPartStart:           db 0
P4SPartStart:           db 0
P4PartType:             db 0
P4CPartEnd:             db 0
P4HPartEnd:             db 0
P4SPartEnd:             db 0
P4LBAStart:             dd 0
P4LBACount:             dd 0

BOOTSIG:              dw 0xaa55
