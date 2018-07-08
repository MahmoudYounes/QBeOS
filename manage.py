#!/usr/bin/python
"""
script that manages BeOs development operations
"""

import os, sys, subprocess, shutil


CURRENT_DIR = os.getcwd()
BOOTLOADER_DIR = "{}/bootLoader/".format(CURRENT_DIR)
BUILD_DIR = "{}/build/".format(CURRENT_DIR)
SUPPORTED_COMMANDS = ['build', 'run', 'help', 'clean']

def executeCommand(command):
	"""
	execute a shell command in linux/unix environment
	"""
	process = subprocess.Popen(command, shell=True, stderr=subprocess.PIPE)
	while True:
		out = process.stderr.read(1)
		exit_code = process.poll()
		if out == '' and exit_code != None:
			exit(exit_code)
		if out != '':
			sys.stdout.write(out)
			sys.stdout.flush()

def help():
	"""
	"""
	print "to use this script: python manage.py <command> or ./manage.py"
	print "where command can be:"
	print "\t\thelp  : to print this help message."
	print "\t\tbuild : to build the flat binary of the OS."
	print "\t\trun	 : to run os in bochs."
	print "\t\tclean : to remove build files."

def clean():
	"""
	"""
	if os.path.isdir(BUILD_DIR):
		shutil.rmtree(BUILD_DIR)
	if os.path.exists("BeOs.img"):
		os.remove("BeOs.img")

def build():
	"""
	for info about build commands check this answer: https://stackoverflow.com/a/33619597
	"""
	clean()
	os.mkdir(BUILD_DIR)

	"""
	"""

	executeCommand("""
		nasm -g -f elf32 -F dwarf -o build/boot.o bootLoader/bootloader.asm;
		ld -melf_i386 -Ttext=0x7c00 -nostdlib --nmagic -o build/boot.elf build/boot.o;
		objcopy -O binary build/boot.elf build/boot.bin;

		nasm -g -f elf32 -F dwarf -o build/kernel.o src/kernel.asm;
		ld -melf_i386 -Tlinker.ld -nostdlib --nmagic -o build/kernel.elf build/kernel.o;
		objcopy -O binary build/kernel.elf build/kernel.bin;

		dd if=/dev/zero of=BeOs.img bs=512 count=2880;
		dd if=build/boot.bin of=BeOs.img bs=512 conv=notrunc;
		dd if=build/kernel.bin of=BeOs.img bs=512 seek=1 conv=notrunc;
		""")

def run():
	if os.path.isdir(BUILD_DIR):
		executeCommand("bochs -f bochsrc.txt")
	else:
		print "Build directory was not found. please build the os first."


def main():
	"""
	"""
	if len(sys.argv) == 1:
		printUsage()
		exit(0)

	command = sys.argv[1]
	if command in SUPPORTED_COMMANDS:
		globals()[command]()
	else:
		print "command not supported."
		help()
	

if __name__ == "__main__":
	main()
