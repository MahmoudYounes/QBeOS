; think of this code as code added on floppy disk.
BITS 16

; this line tells the assembler where to place the data and code in correct locations
; this line tells the bootloader where to assume it is loading.
ORG 0x7C00

jmp _start
nop

; params for bootsector on floppy disk. check: maverick-os.dk/filesystemformats/fat16_filesystem.html
OEM:				db "BeOs    "		; 8		OS Name
SectorSize:			dw 0x200			; 2 	bytes per sector 
SectorPerCluster:	db 1				; 1 	sectors per cluster 
ReservedSector:		dw 1				; 2 	sector 0 is reserved so normaly this is 1
FatCount:			db 2				; 1 	number of FAT on this media 
RootSize:			dw 0x200			; 2 	number of entries in root FAT 
TotalSectors:		dw 2880				; 2 	total number of sectors in this media 
MediaDescript: 		db 0xF0				; 1	    code that describes the floppy
SectorsPerFat:		dw 9				; 2     number of sectors in each FAT 
TrackSector:		dw 9				; 2     number of setors per track 
HeadCnt:			dw 2				; 2
HiddenSectors:		dw 0				; 2
HiddenSectorsHi:	dw 0				; 2
BootDrive:			db 0				; 1     this is the drive number
Rserved:			db 0				; 1
BootSign:			db 0x29				; 1
VolumeId:			db "seri"			; 4
VolumeLabel:		db "MYVOLUME   "	; 11
FSType:				db "FAT16   "		; 8
Cylinders:			db 0x0 				; 1		number of cylinders in drive
SectorsPerTrack:	db 0x0 				; 1 	number of sectors per track
bootFailureMsg:		db "Booting sequence failed", 0
bootLoadingMsg:		db "loading...", 0

_start:
	cli
	; preserving drive number and setting up registers for usage
	mov [BootDrive], dl
	xor ax, ax
	mov ds, ax
	mov es, ax
	; setting up the stack. stack grows downwards
	mov ss, ax
	mov sp, 0x7C00
	sti
	; print loading msg
	pusha
	push bootLoadingMsg
	call printf
	popa
	; reading current disk parameters [this part will be useful when moving to iso instead of floppy]
	pusha
	call readDiskParameters
	popa
	; loading second stage bootloader
	pusha
	mov ax, 2
	push ax
	mov ax, 0x1000
	mov es, ax
	call readDiskSector
	jmp kmain
bootFailure:
	pusha
	push bootFailureMsg
	call printf
	popa
bootloaderEnd:
	cli
	hlt

; ============= Functions ============= ;
; Function to print to screen
; args: address of first character
printf:				; function preparation
	pop bp 			; poping old ip in bp
	pop si 			; poping arguments
	push bp			; re-adding the old ip
loop_printf:		; printing loop
	lodsb
	cmp al, 0
	je endPrintf
	mov ah, 0x0e
	int 0x10
	jmp loop_printf
endPrintf:
	ret

; Function to read floppy disk parameters.
; args: none
readDiskParameters:
	; zero out registers
	xor ax, ax
	mov dx, ax
	; arguments for interrupt
	mov dl, [BootDrive]
	mov ah, 8
	int 13h
	jc bootFailure
	mov [Cylinders], ch
	mov [SectorsPerTrack], cl
endReadDiskParameters:
	ret

; Function to read from disk. default 3 retries and fail boot sequence afterwards
; args: the number of sector to read in lba mode
readDiskSector:
	pop bp 			; saving old ip
	pop ax			; param lba
	mov si, ax 		; preserving lba in si
	mov di, 0
loopReadDiskSector:
	; reseting floppy drive
	mov dl, [BootDrive]
	xor ax, ax
	int 13h
	jc bootFailure

	mov ax, si
	xor dx, dx
	; (dx) Sector = (LBA(ax) mod SectorsPerTrack(18)) + 1
	mov bx, [SectorsPerTrack]
	div bx
	inc dx
	push dx
	; ax has lba / sectorsPerTrack
	; (ax) Cylinder = (LBA / SectorPerTrack) / NumHeads
	; (dx) Head = (LBA / SectorsPerTrack) mod NumHeads
	xor dx, dx
	mov bx, [HeadCnt] 
	div bx
	push ax
	push dx
	; top 3 words in stack are head, cylinder, sector respectively
	; setting head  and drive number
	pop ax
	mov dh, al
	mov dl, [BootDrive]
	; setting cylinder number
	pop ax
	mov ch, al
	; setting sector number
	pop ax
	mov cl, al
	; setting interrupt arguments
	mov al, 1
	mov ah, 2
	mov bx, 0
	; reading
	cmp di, 3
	je bootFailure
	inc di
	int 13h
	jc loopReadDiskSector
	ret

times 510 - ($ - $$) db 0
dw 0xaa55

