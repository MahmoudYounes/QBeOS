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
	process = subprocess.Popen(command, shell = True, stderr = subprocess.PIPE)
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
	print "to use this script: python make.py <command>"
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

def build():
	clean()
	os.mkdir(BUILD_DIR)
	executeCommand("nasm -f bin -o build/boot.bin bootLoader/bootloader.asm; nasm -f bin -o build/kernel.bin src/kernel.asm;")
#	executeCommand("(dd if=build/boot.bin bs=512")

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
