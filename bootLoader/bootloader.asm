; boot code to boot the kernel from iso9660 compliant CD
; boot sector size is 2kb ~ 2048 bytes
; writing the boot sector should be done using EL torito specification with no rock ridge or juliet support

[BITS 16]
[ORG 0x0000]
jmp _start

; boot loader data
BootDrive:					db 0
BootFailureMsg:				db "Booting sequence failed", 0
BootLoadingMsg:				db "loading...", 0
WelcomeMsg:					db "Welcome to BeOS!", 0
BytesPerSector:				dw 0

; kernel info
KernelName:					db "BOOT.BIN", 0x3b, 0x31 
KernelLBA:					dd 0
KernelLength:				dd 0

; gdtr
gdtrContent: 	dw 0    ; limit (size)
			 	dd 0    ; base  (address in memory)

; DAP buffer
DAP:						db 10h			; DAP size (disk address packet)
							db 0			; unused
dapNumSectors:				dw 0			; num sectors
dapBufferOffset:			dw 0			; offset for buffer
dapBufferSegment:			dw 0			; segment for buffer
dapSectorNumL:				dd 0			; absolute sector number low
dapSectorNumH:				dd 0 			; absolute sector number high

global _start
_start:
	; setting up segments and offsets
	cli
	mov ax,0x07c0							
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax

	xor ax,ax
	mov ss,ax
	mov sp,0xffff	
	sti

	; far jmp to make sure that cs and ip are the correct values
	; 5 = 1 opcode + 2 segment + 2 offset
	jmp 0x07c0:$ + 5
	
	; preserving boot drive number
	mov BYTE[BootDrive], dl

	; print loading msg
	mov si, BootLoadingMsg
	call func_printf

	; resetting boot drive
	mov dl, [BootDrive]
	call func_ResetDisk

	; TODO: Check if int 13h extension methods 40h-48h exist and supported by BIOS. if not bootFailure
	;

	; reading primary volume descriptor to locate the kernel files
	; first 32kb (16 sectors) are empty start at sector 16
	mov eax, 100h
	mov es, eax
	xor edi, edi
	call func_readPrimaryVolumeDescriptor

	; search for kernel file	
	mov eax, 100h
	mov es, eax
	xor edi, edi
	call func_LocateKernelImage

	; call func_LoadKernel

	; pushad
	; call func_EnableA20
	; popad



	; assigning right address to gdt
	mov word [gdtrContent], 0xffff		; 4 GB size
	mov word [gdtrContent + 2], 0x0000	; base address
	mov word [gdtrContent + 4], 0x0800	; base address

	; creating 3 entries in the GDT for now
	; null descriptor
	mov ax, 0
	mov es, ax
	mov di, 0x0800
	mov cx, 4
	rep stosw

	; code descriptor (4 GB)
	mov ax, 0xffff 	; limit 0 -> 15 = ffff
	stosw
	mov ax, 0		; base 16 -> 31 = 0
	stosw
	mov al, 0		; base 32 -> 39 = 0
	stosb
	mov al, 0x9a	; access byte 40 -> 47 = 9a = 10011010
	stosb
	mov al, 0xcf	; flags + limit 48 -> 55 = cf = 1100 1111
	stosb
	mov al, 0 		; base 56 -> 64 = 0 
	stosb

	; data descriptor (4 GB)
	mov ax, 0xffff 	; limit 0 -> 15 = ffff
	stosw
	mov ax, 0		; base 16 -> 31 = 0
	stosw
	mov al, 0		; base 32 -> 39 = 0
	stosb
	mov al, 0x92	; access byte 40 -> 47 = 9a = 10010010
	stosb
	mov al, 0xcf	; flags + limit 48 -> 55 = cf = 1100 1111
	stosb
	mov al, 0 		; base 56 -> 64 = 0 
	stosb

	; loading gdt
	lgdt [gdtrContent]

	;clear screen
	xor ax, ax
	mov al, 02h
	int 10h

	pushad
	push WelcomeMsg
	call func_printf
	popad

	cli
	hlt
bootFailure:
	pushad
	push BootFailureMsg
	call func_printf
	popad
bootloaderEnd:
	cli
	hlt

; ============= Functions ============= ;

; ========================================
; Function to print to screen
; args: 
; si: address of first character in stack
; ========================================
func_printf:
loop_printf:		; printing loop
	lodsb
	cmp al, 0
	je endPrintf
	mov ah, 0x0e
	int 0x10
	jmp loop_printf
endPrintf:
	ret

; ========================================
; Reset the drive passed in as argument
; args: 
; dl: drive number as byte in stack
; ========================================
func_ResetDisk:
	mov cx, 3
rd_loop_trials:
	mov ah, 0
	int 13h
	jc rd_ResetFail
	ret
rd_ResetFail:
	dec cx
	cmp cx, 0
	je bootFailure
	jmp rd_loop_trials

; ========================================
; Read Sector from cd rom
; args: 
; cx: number of sectors to read (2 bytes), 
; di: offset of buffer (2 bytes), 
; es: segment of buffer (2 bytes)
; ebx: absolute number of start sector to read (LBA) (4 bytes)
; ========================================
func_ReadISOSector:
	; filling DAP
	pushad

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

; ========================================
; Function that reads the primary volume descriptor from the cd
; args:
; es: segment to load the pvd in
; di: offset to load the pvd in
; after the end of this function, the PVD will be at ds:100h in memory
; ========================================
func_readPrimaryVolumeDescriptor:
rpvd_read:
	pushad
	
	; PVD is always at sector 16 (10h) and is always 1 sector
	mov bx, 10h
	mov cx, 1
	call func_ReadISOSector

	mov al, BYTE[es:di]
	cmp al, 1
	je rpvd_ret
	jmp bootFailure
rpvd_ret:
	popad
	ret

; ========================================
; function to locate the kernel Image
; args:
; es: segment in which the pvd is loaded
; di: offset of which the pvd is loaded
; after this function completes the LBA that contains the file will be in KernelLBA and size will be in KernelLength
; ========================================
func_LocateKernelImage:
	pushad
	mov bx, 100h
	mov edx, [bx + 158]				; LBA of boot record
	pushad
	;H2H1 sector number highest 32 bits
	mov edi, 0
	;L2L1 sector number lowest 32 bits
	mov ecx, edx

	; buffer segment
	mov dx, 0

	; buffer offset
	mov bx, 100h

	; number of sectors to read
	mov ax, 1
	
	call func_ReadISOSector
	popad

	xor cx, cx
	mov bx, 100h			; reset buffer
; TODO: there is a bug here (pointer out of buffer bounds "need to fix this")
loopLocateKernelFile:
	mov dl, [bx]			; size of entry in buffer
	mov cl, [ds:bx + 32]	; file identifier length
	mov si, bx
	add si, 33
	mov ax, ds
	mov es, ax
	mov di, KernelName
cmpStr:
	cmp cl, 0
	je fileFound
	dec cl
	cld
	cmpsb
	jne nextEntry
	jmp cmpStr
nextEntry:
	add bl, dl
	jmp loopLocateKernelFile

fileFound:
	mov eax, [bx + 2]
	mov [KernelLBA], eax
	mov eax, [bx + 10]
	mov [KernelLength], eax
	ret
; ========================================

; function to read kernel and load it at 0x0000:0x9000
; args: none
func_LoadKernel:
	pushad
	;H2H1 sector number highest 32 bits
	mov edi, 0
	;L2L1 sector number lowest 32 bits
	mov ecx, [KernelLBA]		

	; buffer segment
	mov dx, 0

	; buffer offset
	mov bx, 9000h

	; number of sectors to read
	mov ax, 1
	
	call func_ReadISOSector
	popad
	ret
; ========================================

; function to enable A20 if not enabled
; args: none
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


TIMES 2046-($-$$) db 0
dw 0xAA55