ASM = nasm

build: kloader.asm
    # -w+orphan-labels causes any orphan labels to give a warning. an orphan label is <label> without :
	$(ASM) -Werror -w+orphan-labels -g -f bin -o $(BLD_DIR)/kloader.bin kloader.asm
