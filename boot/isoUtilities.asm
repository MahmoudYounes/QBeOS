; ========================================
; Function that reads the primary volume descriptor from the cd
; args:
; es: segment to load the pvd in
; di: offset to load the pvd in
; after the end of this function, the PVD will be at ds:100h in memory
; ========================================
func_ReadPrimaryVolumeDescriptor:
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
