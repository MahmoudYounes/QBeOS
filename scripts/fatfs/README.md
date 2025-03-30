# Summary
fatfs is a tool to help build fatfs disk images. The main purpose of building
this tool is to create QBeOS bootable fat32 disk images.

# Current State
I did a minute survey for how to create a bootable FAT32 image for QBeOS
from multiple files. It was not an easy or straight forward task. The current
flow to do so are:
- create a disk image file using dd.
```bash
dd if=/dev/zero of=disk.img bs=512 count=1024
```
- format this disk image using fdisk to create an MsDOS or GPT partition table.
- create a fat32 file system using mkfs.fat -F 32. 
- use mtools (mcopy -i disk.img <file>) to copy files to this disk image.
so far so good. The problem is that I couldn't find a tool that write a MBR
with a provided 512 bytes binary while setting also the correct MBR data
expected to be in the MBR of a FAT fs.
There is unetbootin but this is not a scriptable methodology and can't be used
in automated build systems. It is also not developed for generic OSes as it
adds linux files to the generated FAT32 FS. It also can't generate a disk image
although it writes one to a /dev file.

# Solution
This tool does multiple things but mainly solves two issues:
- like mkisofs tool this tool will take a destination to a directory.
- it will create a fat32 hard disk img file from this directory with one partition.
- the directory will contain a MBR used to boot whatever can be booted from
  this directory.
- it will write a correct MBR to this image with the correct data.
- burn the generated image to any device.

# Caveats
- this tool maybe or maybe not be tightly coupled with QBeOS.
- this tool is not a generic tool to create FAT drives or images. It only supports
  one usecase which is to create a QBeOS boot image formatted as FAT32. However,
  this tool can be extended to be a general purpose tool for FAT filesystem.

# Testing
The FAT32 parser folder added to this repo is used to validate the images
created.

