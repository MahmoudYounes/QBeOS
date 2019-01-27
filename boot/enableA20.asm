; ========================================
; function to enable A20 if not enabled
; credits to osdev
; args: none
; ========================================
func_EnableA20:
	pushad
	cli
	mov cx, 0						   ; counter to check which method we have used

checkIfEnabled:
	; first let's check if a20 is enabled
	; if not enabled carry flag will be set
	clc
	call func_testA20
	jne EnableA20End


EnableA20:
	cmp cx, 0
	je BiosEnable
	cmp cx, 1
	je KeyboardControllerEnable
	cmp cx, 2
	je FastGateEnable
	jmp bootFailure

BiosEnable:
	inc cx
	clc
	call func_biosEnableA20
	jc EnableA20
	jmp checkIfEnabled

KeyboardControllerEnable:
	inc cx
	call func_keyboardControllerEnableA20
	jmp checkIfEnabled

FastGateEnable:
	inc cx
	in al, 0x92
	test al, 2
	jnz checkIfEnabled
	or al, 2
	and al, 0xFE
	out 0x92, al
	jmp checkIfEnabled

EnableA20End:
	popad
	ret


; ========================================
; function to check if A20 line is enabled
; or no.
; args: none
; if enabled carry flag is set
; ========================================
func_testA20:
	pushad
	
	cli

	xor ax, ax 				; ax = 0
	mov es, ax

	not ax 					; ax = 0xFFFF
	mov ds, ax

	mov di, 0x0500
	mov si, 0x0510

	mov al, byte [es:di]
	push ax

	mov al, byte [ds:si]
	push ax

	mov byte [es:di], 0x00
	mov byte [ds:si], 0xFF

	cmp byte [es:di], 0xFF

	pop ax
	mov byte [ds:si], al

	pop ax
	mov byte [es:di], al
	
	popad
	ret

; ========================================
; function to enable A20 line with bios
; interrupts.
; args: none
; if error carry flag is set
; ========================================
func_biosEnableA20:							; BIOS int method
	pushad

	mov ax, 2403h
	int 15h
	jb  biosError                  				; INT 15h is not supported
	cmp ah, 0
	jnz biosError                  				; INT 15h is not supported
	
	mov ax, 2402h               
	int 15h
	jb  biosError               				; couldn't get status
	cmp ah, 0
	jnz biosError               				; couldn't get status
	
	cmp al, 1
	jz  biosEnableEnd              				; A20 is already activated
	
	mov ax, 2401h
	int 15h
	jb  biosError   		            		; couldn't activate the gate
	cmp ah, 0	
	jnz biosError	 	   		        		; couldn't activate the gate
	jmp biosEnableEnd
biosError:
	stc
biosEnableEnd:
	popad
	ret

; ========================================
; function to enable A20 line with 
; keyboard controller manipulation
; args: none
; ========================================
func_keyboardControllerEnableA20:
	pushad

	call func_a20WaitAl2
	mov al, 0xad
	out 0x64, al

	call func_a20WaitAl2
	mov al, 0xd0
	out 0x64, al

	call func_a20WaitAl1
	in al, 0x60
	push eax

	call func_a20WaitAl2
	mov al, 0xd1
	out 0x64, al

	call func_a20WaitAl2
	pop eax
	or al, 2
	out 0x60, al

	call func_a20WaitAl2
	mov al, 0xae
	out 0x64, al

	call func_a20WaitAl2
	sti
controllerEnd:
	popad
	ret

; ========================================
; function to await keyboard controller
; port 
; args: none
; ========================================
func_a20WaitAl2:
	in      al, 0x64
	test    al, 2
	jnz     func_a20WaitAl2
	ret

; ========================================
; function to await keyboard controller
; port 
; args: none
; ========================================
func_a20WaitAl1:
	in      al,0x64
	test    al,1
	jz      func_a20WaitAl1
	ret