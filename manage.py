#!/usr/bin/python
"""
backup : nasm -g -f elf32 -F dwarf -o build/boot.o bootLoader/boot.asm;
	ld -melf_i386 -Tlinker.ld -nostdlib --nmagic -o build/boot.elf build/boot.o;
	objcopy -O binary build/boot.elf build/boot.bin;


script that manages BeOs development operations

TODO: move to make
TODO: adjust build commands to accomdate for failures
"""

import os, sys, subprocess, shutil


CURRENT_DIR = os.getcwd()
BOOTLOADER_DIR = "{}/bootLoader/".format(CURRENT_DIR)
BUILD_DIR = "{}/build/".format(CURRENT_DIR)
BIN_DIR = "{}/bin/".format(CURRENT_DIR)
ISOROOT_DIR = "{}/iso_root/".format(CURRENT_DIR)

SUPPORTED_COMMANDS = ['build', 'run', 'help', 'clean', 'debug']

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
	print "\thelp  : to print this help message."
	print "\tbuild : to build the ISO file of the OS."
	print "\trun   : to run os in bochs."
	print "\tclean : to remove build files."

def clean():
	"""
	"""
	if os.path.isdir(BUILD_DIR):
		shutil.rmtree(BUILD_DIR)
	if os.path.isdir(ISOROOT_DIR):
		shutil.rmtree(ISOROOT_DIR)
	if os.path.isdir(BIN_DIR):
		shutil.rmtree(BIN_DIR)

def build():
	"""
	for info about build commands check this answer: https://stackoverflow.com/a/33619597
	"""
	clean()
	if not os.path.exists(BUILD_DIR):
		os.mkdir(BUILD_DIR)
	if not os.path.exists(ISOROOT_DIR):
		os.mkdir(ISOROOT_DIR)
	if not os.path.exists(BIN_DIR):
		os.mkdir(BIN_DIR)
	executeCommand("""
	nasm -g -f bin -o build/bootloader.bin bootLoader/bootloader.asm;
	
	nasm -g -f elf32 -F dwarf -o build/kernel.o src/kernel.asm;
	ld -melf_i386 -Tlinker.ld -nostdlib --nmagic -o build/kernel.elf build/kernel.o;
	objcopy -O binary build/kernel.elf build/kernel.bin;


	cp build/bootloader.bin iso_root/
	cp build/kernel.bin iso_root/

	mkisofs -c bootcat -b bootloader.bin -no-emul-boot -boot-load-size 4 -o ./bin/BeOs.iso ./iso_root
	""")

def run():
	if os.path.isdir(BUILD_DIR):
		executeCommand("bochs -f bochsrc.txt")
	else:
		print "Build directory was not found. please build the os first."

def debug():
	if os.path.isdir(BUILD_DIR) and os.path.isdir(BIN_DIR) and os.path.exists("qemu_dbg.gdb"):
		executeCommand("gdb -x qemu_dbg.gdb")
	else:
		print "required directories not found. please build the os first."

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
