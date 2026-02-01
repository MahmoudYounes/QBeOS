ASM = nasm
ASM_FILES = $(wildcard *.asm)
ASM_MAIN_FILE = earlyloader.asm
ASM_OUTPUT_FILE = earlyloader.bin 

build: $(ASM_FILES)
    # -w+orphan-labels causes any orphan labels to give a warning. an orphan label is <label> without :
	$(ASM) -Werror -w+orphan-labels -g -f bin -o $(BLD_DIR)/$(ASM_OUTPUT_FILE) $(ASM_MAIN_FILE)
