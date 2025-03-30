;===================================================================
;				        QBeOS KernelLoader Code
;				        =======================
;
; boot code to load the QBeOS kernel from iso 9660 compliant CD.
; works on 8086 architecture, 32 bits, single CPU
; this is the second stage bootloader so we are not limited by
; any sizes here.
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
  cli
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
  
  ; restore the boot drive
  pop ax
  mov [BootDrive], al
  xor eax, eax

  ; reset the stack
	mov ax, 0x02d0
	mov ss, ax
	xor ax, ax
	mov sp, 0x4eff 

  ; set interrupts to use BIOS
  sti
  
  call func_biosClearScreen

  mov si, LoaderHelloMsg
  call func_biosPrint

  ; far jmp to make sure that cs and ip are the correct values
  ; 5 = 1 opcode + 2 segment + 2 offset
	jmp 0h: $ + 5
 
  ; resetting boot drive
;	mov dl, [BootDrive]
;	call func_ResetDisk
  
  jmp $



;  call func_EnableA20

%include "../drivers/screen/screen.asm"
;%include "./enableA20.asm"
;%include "../isofs/isoUtilities.asm"

BootDrive: db 0
LoaderFailuresMsg: db "Booting sequence failed in second stage", 0
LoaderHelloMsg: db "Loading QBeOS...", 0

;; Screen State
currentCursorPosition:		db 0; the current position of the cursor state
whiteOnBlackConst:			  equ 0x0f; const added to video memory
rowsLimit:					      equ 80
colsLimit:					      equ 25


