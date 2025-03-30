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
; jmp _start ; this jump will be handled by the tool that will write the MBR
times 63 db 0
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
	;jmp 0h: $ + 5

  ; preserving boot drive number
	mov BYTE[BootDrive], dl

  ; print loading msg
	mov si, BootLoadingMsg
	call func_biosPrint
 
  ; resetting boot drive
	mov dl, [BootDrive]
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

  ; don't care which partition is active let's just loop on logical blocks
  mov ecx, 1 ; currPartIndex 
loopBlock:
  mov edx, [PartBegin + ecx*9] ; lba start  
	mov eax, [MBRBufAddress]     ; buffer to load the block
	mov es, eax
	xor edi, edi
  mov ebx, edx
	call func_ReadLBA

loopDirs:
	mov eax, [MBRBufAddress]
	mov es, eax
	xor edi, edi

func_LocateKernelLoader:
	; this address has the LBA of root directory.
  ; byte offset 156 contains the root directory record.
  ; byte offset 2 contains LBA of this root directory.
	; preserving the LBA location in ebx (4bytes LSB LBA) before messing up es
	mov ebx, [es:di + 158]

	; preparing to read new sector
	; setting up new buffer for the new volume
	xor di, di  				; reseting the offset pointer
	mov ax, [BLBufPointer]
	mov es, ax
	mov cx, 1
	call func_ReadLBA

	xor di, di			; reset buffer pointer
	mov bx, di			; bx used as a pointer to beginning of the directory entry. 
                  ; while, di is used to compare names.

	;; TODO: for separation of concerns, this part should be extracted to 
  ;; isoUtilities library and be as generic as possible.
	;; an abstraction like func_LocateFile with arguments being pointer to buffer is in need.
loopLocateKernelLoaderFile:
	xor edx, edx
	mov dl, BYTE [es:bx]			; size of entry in buffer
	xor ecx, ecx
	mov cl, BYTE [es:bx + 32]		; file identifier length
	add di, 33						; first byte of file identifier
	mov si, KernelLoaderName
cmpStr:
	cmp cx, 0					    ; loop ended and file found
	je fileFound
	dec cx
	cld
	cmpsb
	jne nextEntry
	jmp cmpStr
nextEntry:
	add bx, dx		
	mov di, bx
	jmp loopLocateKernelLoaderFile
fileFound:
	mov eax, DWORD [es:bx + 2]
	mov [KernelLoaderLBA], eax
	mov eax, DWORD [es:bx + 10]
	mov [KernelLoaderLength], eax



func_LoadKernelLoader:
; calculating how many sectors to read
	xor eax, eax
	mov ebx, eax
	mov ecx, eax

	mov eax, [KernelLoaderLength]
	add eax, 0x0800
	mov bx, 0x0800
	div bx

	mov cl, al
	mov ax, 0x0840
	mov es, eax
	xor di, di
	mov ebx, [KernelLoaderLBA]
	call func_ReadLBA
  jmp bootFailure

  ; store boot drive for second stage
  xor eax, eax
  mov al, [BootDrive]
  push ax
  jmp 0x08400

	;call func_DiscoverMemory

  ;call func_DiscoverPCI

	;call func_PrepareGDT

	;call func_EnableProtectedModeAndJmpKernel

    cli
    hlt
bootFailure:
	mov si, BootFailureMsg
	call func_biosPrint
  jmp bootloaderEnd
bootloaderEnd:
	cli
	hlt

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

BootFailureMsg:				      db "Boot failed", 0
;MemDiscoveryFailureMsg:	    db "Memory discover failed", 0
BootLoadingMsg:				      db "laoding", 0
;PCIErrorMsg:                db "Couldn't discover PCI", 0
;BytesPerSector:				      dw 0

; kernel loader info
KernelLoaderName:					db "KLOADER.BIN", 0x3b, 0x31
KernelLoaderLBA:					dd 0
KernelLoaderLength:				dd 0

; PVD begining address
MBRBufAddress:	dw 0x0050

; Bootloader buffer pointer
BLBufPointer:	dw 0x00d0

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
TIMES 440 - ($ - $$) db 0

; MBR Data
DISKSIG:              dd 0
RES:                  dw 0x0
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
