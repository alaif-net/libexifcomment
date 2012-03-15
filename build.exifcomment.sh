#!/bin/bash

#CrossCompiler settings.
export NDK_TOP=/home/user01/work/android-ndk-r7b
export SYSROOT=$NDK_TOP/platforms/android-8/arch-arm
export CFLAGS="-march=armv5te -msoft-float "
export CC="$NDK_TOP/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/arm-linux-androideabi-gcc -mandroid --sysroot=$SYSROOT"

INSTALL_TARGET=/home/user01/work/android_libs/exifcomment/target
ARCHIVE=libexif-0.6.20.tar.gz
WORKDIR=`basename $ARCHIVE .tar.gz`

$CC $CFLAGS -I$INSTALL_TARGET/usr/local/include -c exif_comment.c
$CC $CFLAGS -L$INSTALL_TARGET/usr/local/lib -o libexifcomment.so -shared exif_comment.o -lexif -lm
