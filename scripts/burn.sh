#!/bin/bash

# A script that burns QBeOS.iso and the bootloader on a bootable CD. Useful
# to create a bootable CD image on real hardware or a bootable HDD image
# on a hard disk for motherboards that support El-Torrito boot specification.
#
# This script is untested. I don't have hardware that supports this boot protocol.

ISO_NAME="QBeOS.iso"
BOOTLOADER_NAME="bootloader.bin"

whoami=$(whoami)
if [[ $whoami != "root" ]]; then
  echo "need to be root to run the script"
fi

if [[ $# != 1 ]]; then
  echo "invalid usage"
fi

TARGET_DEV=$1
SCRIPT_PATH=$(dirname $0)
ISO_PATH=$SCRIPT_PATH/../bin/$ISO_NAME
BOOTLOADER_PATH=$SCRIPT_PATH/../build/$BOOTLOADER_NAME

if [[ ! -f $ISO_PATH ]]; then
  echo "can't fine ISO in $ISO_PATH"
  exit 1
fi

if [[ ! -f $BOOTLOADER_PATH ]]; then
  echo "can't fine ISO in $BOOTLOADER_PATH"
  exit 1
fi


dd if=$ISO_PATH of=$TARGET_DEV bs=512
sync

dd conv=notrunc if=$BOOTLOADER_PATH of=$TARGET_DEV bs=512 count=1 
sync
