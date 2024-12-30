    .file "usermode.S"
    .text
    .global JumpUsermode
    .type JumpUsermode,@function
    .intel_syntax noprefix
    .code32
JumpUsermode:
  mov ax, 0x23 
	mov ds, ax
	mov es, ax 
	mov fs, ax 
	mov gs, ax 

  lea edx, [TestUserMode]
  mov ecx, esp
	sysexit

