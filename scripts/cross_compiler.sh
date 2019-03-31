#!/bin/bash

# in this script, we will download and build our gcc cross compiler

# warning message
echo "this script builds gcc cross compiler from latest packages. this script comes"
echo "with BeOS and is required to be executed to do further development."
echo "execute this script on your own responsibility as it provides no warrenty."
echo "apt will be used in this script to download some packages (read apt"
echo "messasges to understand what will be downloaded) so it will prompt you to"
echo "enter your sudo password"
read -p "press any key to continue..."
clear

# helpful functions
createDirIfNotExist () {
    if [ ! -d $1 ]; then
        mkdir $1
        if [ $? -ne 0 ]; then
            echo "Failed to create directory: $0"
            exit 1
        fi
    fi
}

# setup global vars
export PREFIX="$HOME/opt"
export TARGET=i686-elf
export TMP_DIR="$HOME/tmp"
export BINUTILS_DIR="$PREFIX/binutils"
export GCC_DIR="$PREFIX/gcc"
export PATH="$PREFIX/bin:$PATH"

# creating required folders
echo "creating $PREFIX"
createDirIfNotExist $PREFIX
echo "creating $TMP_DIR"
createDirIfNotExist $TMP_DIR
echo "creating $BINUTILS_DIR"
createDirIfNotExist $BINUTILS_DIR
echo "creating $GCC_DIR"
createDirIfNotExist $GCC_DIR

echo "downloading packages..."

cd $TMP_DIR
# download all cross compiler dependiencies
# check the dependiencies version in this url: https://wiki.osdev.org/GCC_Cross-Compiler
sudo apt install -y build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo libcloog-isl-dev libisl-0.18-dev git subversion

# downloading latest version of binuils through git
git clone git://sourceware.org/git/binutils-gdb.git binutils-src

# downloading latest version of gcc
svn checkout svn://gcc.gnu.org/svn/gcc/trunk gcc-src

clear
echo "staring build..."
cd $BINUTILS_DIR
$TMP_DIR/binutils-src/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
make install


which -- $TARGET-as
if [ $? -ne 0 ]; then
    echo $TARGET-as is not in PATH
    exit 1
fi

cd $GCC_DIR
$TMP_DIR/gcc-src/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc
