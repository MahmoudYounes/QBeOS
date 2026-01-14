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
- this tool may be or may not be tightly coupled with QBeOS.
- this tool is not a generic tool to create FAT drives or images. It only supports
  one usecase which is to create a QBeOS boot image formatted as FAT32. However,
  this tool can be extended to be a general purpose tool for FAT filesystem.

# Testing
The FAT32 parser folder added to this repo is used to validate the images
created.

# Fat 32
- file parts are called clusters
- a directory is a special file. The data of that file is a 32 byte dir entries
- on Fat32 RootDirEnt is always 0
- with MS FAT32 implementation:
  - num fats must be 2 
  - reserved sectors must be 32 secotrs
  - there exists a sector that is called FSInfo that placed somewhere. It
    contains 
- Each Dir Entry is a 32 byte (not bit)
- On Fat32 the RootEntCnt is 0 because  
## Fat32 structure
+---------------+
|  boot sector  |
+---------------+
|  nn secotrs   |
|  reserved     |
|  app specific |
|  (2nd BL)     |
+---------------+
|  Fat          |
|               |
+---------------+
|  Data         |
|               |
+---------------+

## Boot Sector
The boot sector contains all the required information about the FS.
The structure of this can be seen in the fat32.go file.

## FAT Entry
fat entry is 32 bits (4 bytes) with only 28 bits addressable. The 4 MSBs are
reserved and should be kept as they are.
The entry index is used as a cluster number. The entry value is used as a 
pointer to the number of the next cluster.

## FAT32 Access algorithm
Always start at the RootClus number. Resolve this cluster address and then
read SecPerClus sectors. This is the root directory entry. It contains
a 32 byte consecutive directory entries that could either be directories or files.

read dir:
For each of the entries, read the cluster number hi and lo. This is the first 
cluster of the file/dir. 

If it is a file, then you can read the file cluster right away. to get the next
cluster you resolve the FAT entry of the cluster and then read this fat entry.
If it contains 0x0fffffff then you know this was the only cluster you needed 
to read, otherwise, you resolve the cluster number address and then read it
and then jump to the next entry.

If it is a directory, then you go to the read dir


