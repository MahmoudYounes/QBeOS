;===================================================================
;				        QBeOS BootLoader Code
;				        =====================
;
; boot code to boot the mahcine and load the kernel loader from 
; block devices using LBA. Requires a bios that support this.
; works on 8086 architecture, 32 bits, single CPU.
; boot sector size is 512b.
;
; this file implements a driver layer on top of BIOS IVT
;===================================================================


; ========================================
; Function that reads a sector from disk
; using LBA
;
; args:
; ebx: sector LBA
; es : segment to load the pvd in
; di : offset to load the pvd in
;
; after the end of this function, the PVD will be at es:di in memory
; ========================================
func_ReadLBA:
lba_read:
	pushad

	mov cx, 1
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
; Reset the drive passed in as argument
; args: 
; dl: drive number as byte in stack
; ========================================
func_ResetDisk:
  pushad

	mov cx, 3
rd_loop_trials:
	mov ah, 0
	int 13h
	jc rd_ResetFail
	ret
rd_ResetFail:
	dec cx
	cmp cx, 0
	; TODO: return error codes
  je bootFailure
	jmp rd_loop_trials

  popad
  ret



