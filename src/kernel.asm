[bits 16]
%include "./bootLoader/screen.asm"

global BeOSKernel
BeOSKernel:
    call func_biosClearScreen

    mov si, WelcomeMsg
    call func_biosPrintf
    hlt


WelcomeMsg:					db "Welcome to BeOS!", 0
; screen state
currentCursorPosition:		db 0			; the current position of the cursor state
whiteOnBlackConst:			equ 0x0f		; const added to video memory
rowsLimit:					equ 80
colsLimit:					equ 25