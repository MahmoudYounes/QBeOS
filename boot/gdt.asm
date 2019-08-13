bits 16

func_PrepareGDT:
    pushad
    
    xor     eax, eax
	mov     ax, ds
	shl     eax, 4
	add     eax, gdtData
	mov     [GDT + 2], eax
    cli
	lgdt [GDT]
	sti

    popad
    ret