#!/usr/bin/python
"""
script that manages BeOs development operations

TODO: adjust build commands to accomdate for failures
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
	if os.path.exists("bin/BeOs.iso"):
		os.remove("bin/BeOs.iso")

def build():
	"""
	for info about build commands check this answer: https://stackoverflow.com/a/33619597
	"""
	clean()
	os.mkdir(BUILD_DIR)

	executeCommand("""
		nasm -g -f elf32 -F dwarf -o build/bootloader.o bootLoader/bootloader.asm;
		ld -melf_i386 -Ttext=0x7c00 -nostdlib --nmagic -o build/bootloader.elf build/bootloader.o;
		objcopy -O binary build/bootloader.elf build/bootloader.bin;

		nasm -g -f elf32 -F dwarf -o build/boot.o bootLoader/boot.asm;
		ld -melf_i386 -Tlinker.ld -nostdlib --nmagic -o build/boot.elf build/boot.o;
		objcopy -O binary build/boot.elf build/boot.bin;

		cp build/bootloader.bin iso_root
		cp build/boot.bin iso_root
		mkisofs -R -J -c bootcat -b bootloader.bin -no-emul-boot -boot-load-size 4 -o ./bin/BeOs.iso ./iso_root
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
