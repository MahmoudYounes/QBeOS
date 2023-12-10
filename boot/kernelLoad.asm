;; KernelLoad.asm library contains kernel loading utility.

; ========================================
; function to locate the kernel Image
; args:
; es: segment in which the pvd is loaded
; after this function completes the LBA that contains the file will be in KernelLBA and size will be in KernelLength
; ========================================
func_LocateKernelImage:
	pushad

	; this address has the LBA of root directory; byte offset 156 contains the root directory record. byte offset 2 contains LBA
	; of this root directory
	; preserving the LBA location in ebx (4bytes LSB LBA) before messing up es
	mov ebx, [es:di + 158]

	; preparing to read new sector
	; setting up new buffer for the new volume
	xor di, di  				; reseting the offset pointer
	mov ax, [BLBufPointer]
	mov es, ax
	mov cx, 1
	call func_ReadISOSector

	xor di, di			; reset buffer pointer
	mov bx, di			; bx used as a pointer to beginning of the directory entry. while, di is used to compare names
	;; TODO: for separation of concerns, this part should be extracted to isoUtilities library and be as generic as possible.
	;; an abstraction like func_LocateFile with arguments being pointer to buffer is in need.
loopLocateKernelFile:
	xor edx, edx
	mov dl, BYTE [es:bx]			; size of entry in buffer
	xor ecx, ecx
	mov cl, BYTE [es:bx + 32]		; file identifier length
	add di, 33						; first byte of file identifier
	mov si, KernelName
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
; 0x0000:0x8400
; to avoid overwritting any BIOS Data
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
	mov ax, 0x0840
	mov es, eax
	xor di, di
	mov ebx, [KernelLBA]
	call func_ReadISOSector
	popad
	ret
