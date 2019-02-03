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


	mov di, 0			; reset buffer
	mov bx, di			; bx used to point to beginning of the directory entry. while, di is used to compare names
; TODO: there is a bug here (pointer out of buffer bounds "need to fix this")
; es:di points to the beginning of the root directory entries
; first two entries of the root directory are the .(current dir) and ..(parent dir)
loopLocateKernelFile:
	xor edx, edx
	mov dl, BYTE [es:bx]			; size of entry in buffer
	xor ecx, ecx
	mov cl, BYTE [es:bx + 32]		; file identifier length
	add di, 33						; first byte of file identifier
	mov si, KernelName
cmpStr:
	cmp cx, 0
	je fileFound
	dec cx
	cld
	cmpsb
	jne nextEntry
	jmp cmpStr
nextEntry:
	add bx, dx		
	mov di, bx
	jmp loopLocateKernelFile
fileFound:
	mov eax, DWORD [es:bx + 2]
	mov [KernelLBA], eax
	mov eax, DWORD [es:bx + 10]
	mov [KernelLength], eax

	popad
	ret

; ========================================
; function to read kernel and load it at 
; 0x0000:0x9000
; args: none
; ========================================
func_LoadKernel:
	pushad
	; calculating how many sectors to read
	xor eax, eax
	mov ebx, eax
	mov ecx, eax

	mov eax, [KernelLength]
	add eax, 0x0800
	mov bx, 0x0800
	div bx

	mov cl, al
	mov di, 0x9000
	mov eax, 0
	mov es, eax
	mov ebx, [KernelLBA]
	call func_ReadISOSector
	popad
	ret