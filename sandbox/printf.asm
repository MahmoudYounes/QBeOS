section .text 
global _start
_start:
	mov ax, 3
	int 10h
	mov esi, loadingMsg
	call printf
	jmp endProg

printf:
	mov dx, 0xB800
	mov es, dx
	mov cx, 0
	printf_forEachChar:
		lodsb
		cmp al, 0x00
		je endFunc
		mov di, cx
		mov [es:di], al
		inc cx
		inc cx
		jmp printf_forEachChar
endPrintf:
	ret

endProg:
	mov eax, 1
	mov ebx, 0
	int 80h


section .data
failureMsg:		db "Booting sequence failed... press any key to continue", 0x0
loadingMsg:		db "loading, please wait!"
loadingMsgLen:  db $ - loadingMsg