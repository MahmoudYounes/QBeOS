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
	mov ebx, 10h
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
	
	; this address has the LBA of boot record
	mov edx, [es:di + 158]

	; setting up new buffer for the new volume
	; push es
	; push di
	mov ax, 200h
	mov es, ax
	xor di, di

	; preparing to read new sector
	mov ebx, edx
	mov cx, 1
	call func_ReadISOSector


	xor cx, cx
	mov bx, 0			; reset buffer
; TODO: there is a bug here (pointer out of buffer bounds "need to fix this")
loopLocateKernelFile:
	mov dl, [es:bx]			; size of entry in buffer
	mov cl, [es:bx + 32]	; file identifier length
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
; ========================================
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