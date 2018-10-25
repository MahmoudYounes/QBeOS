; ========================================
; function to enable A20 if not enabled
; args: none
; ========================================
func_EnableA20:
	cli
	mov cx, 0
testA20:
	xor ax, ax ; ax = 0
	mov es, ax

	not ax ; ax = 0xFFFF
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
	
	je EnableA20
	jmp EnableA20End

EnableA20:
	cmp cx, 0
	je EnableA200
	cmp cx, 1
	je EnableA201
	cmp cx, 2
	je EnableA202
	jmp bootFailure

EnableA200:								; BIOS int ,ethod
	inc cx								; we do not want to get back here again
	mov     ax, 2403h
	int     15h
	jb      EnableA20                  	; INT 15h is not supported
	cmp     ah, 0
	jnz     EnableA20                  	; INT 15h is not supported
	
	mov     ax, 2402h               
	int     15h
	jb      EnableA20               	; couldn't get status
	cmp     ah, 0
	jnz     EnableA20               	; couldn't get status
	
	cmp     al, 1
	jz      testA20                 	; A20 is already activated
	
	mov     ax, 2401h
	int     15h
	jb      testA20  		            ; couldn't activate the gate
	cmp     ah, 0	
	jnz     testA20 	   		        ; couldn't activate the gate
	jmp testA20

EnableA201:
	inc cx
	cli
	call    a20wait
	mov     al, 0xAD
	out     0x64, al

	call    a20wait
	mov     al, 0xD0
	out     0x64, al

	call    a20wait2
	in      al, 0x60
	push    eax

	call    a20wait
	mov     al, 0xD1
	out     0x64, al

	call    a20wait
	pop     eax
	or      al, 2
	out     0x60, al

	call    a20wait
	mov     al, 0xAE
	out     0x64, al

	call    a20wait
	sti
	jmp testA20

a20wait:
	in      al,0x64
	test    al,2
	jnz     a20wait
	ret

a20wait2:
	in      al,0x64
	test    al,1
	jz      a20wait2
	ret

EnableA202:
	inc cx
	in al, 0x92
	test al, 2
	jnz after
	or al, 2
	and al, 0xFE
	out 0x92, al
after:
	jmp testA20


EnableA20End:
	ret
