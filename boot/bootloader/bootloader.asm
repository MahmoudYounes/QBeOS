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
times 88 db 0 ; a tool will use this area to write DS for BIOS 
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

  call func_biosClearScreen
  
  mov si, LoadingMsg
  call func_biosPrint

  ; preserving boot drive number
	mov BYTE[BootDrive], dl

  call func_ResetDisk

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
  ; ebx is the counter to the number of LBAs loaded and which LBA we will load 
  xor ebx, ebx
  ; we move the location in memory we will load the second stage BL into
  mov ax, [MBRBufAddress]
	mov es, eax
	xor edi, edi
  ; start with the first sector. sector 0 is the MBR
  mov bl, [SecondStageLBAStart]   
loadStage2:
	call func_ReadLBA
  inc ebx
  add edi, 512
  cmp bl, [SecondStageLBAs]
  jle loadStage2

; important TODO: do we need the boot drive here? 
jumpStage2:
  mov dl, [BootDrive]
  jmp 0h:0x8400 

bootFailure:
	mov si, FailureMsg 
	call func_biosPrint
  jmp bootloaderEnd
bootloaderEnd:
	cli
	hlt
;;;;;;;;;;; end ;;;;;;;;;;;

%include "../earlyloader/disk.asm"
%include "../earlyloader/screen.asm"

BootDrive:					  db 0
LoadingMsg:           db "Loading..", 0
FailureMsg:           db "Failed boot..", 0
; Location of the early loader
MBRBufAddress:        dw 0x0840

SecondStageLBAStart:  db 2
; How many LBAs are there to read for the second stage
SecondStageLBAs:      db 2 

;; DAP buffer
DAP:						      db 10h; DAP size(disk address packet)
							        db 0; unused
dapNumSectors:				dw 0; num sectors
dapBufferOffset:			dw 0; offset for buffer
dapBufferSegment:			dw 0; segment for buffer
dapSectorNumL:				dd 0; absolute sector number low
dapSectorNumH:				dd 0; absolute sector number high

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
