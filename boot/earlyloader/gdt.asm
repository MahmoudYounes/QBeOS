func_PrepareGDT:
    pushad

    cli
	  lgdt [GDT]

    popad
    ret
