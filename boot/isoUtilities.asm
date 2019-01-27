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
