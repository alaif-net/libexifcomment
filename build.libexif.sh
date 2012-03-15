#!/bin/bash

#CrossCompiler settings.
export NDK_TOP=/home/user01/work/android-ndk-r7b
export SYSROOT=$NDK_TOP/platforms/android-8/arch-arm
export CFLAGS="-march=armv5te -msoft-float "
export CC="$NDK_TOP/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/arm-linux-androideabi-gcc -mandroid --sysroot=$SYSROOT"

INSTALL_TARGET=/home/user01/work/android_libs/exifcomment/target
ARCHIVE=libexif-0.6.20.tar.gz
WORKDIR=`basename $ARCHIVE .tar.gz`

if [ ! -d $INSTALL_TARGET ]
then
	echo "Create target directory: $INSTALL_TARGET"
	mkdir $INSTALL_TARGET	
fi

if [ -d $WORKDIR ]
then
	echo "Clear work directory: $WORKDIR"
	rm -rf $WORKDIR
fi

tar xzf $ARCHIVE

cd $WORKDIR

./configure --host=arm-eabi --disable-nls --enable-shared
make && make DESTDIR=$INSTALL_TARGET install
