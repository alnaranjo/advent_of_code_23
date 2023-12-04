#!/bin/sh

if [ -z "$1" ]; then
  echo "Error: missing build target."
  echo "Ussage: ./build.sh <day_x>"
  exit 1
fi

SRC_DIR="$1"
UTILS_DIR="utils"
BUILD_DIR=bin

set echo on

ASSEMBLY="main"

LIB_C_FILESNAMES=$(find ./$UTILS_DIR -type f -name "*.c")
C_FILENAMES=$(find ./$SRC_DIR -type f -name "*.c")

COMPILER_FLAGS="-g -std=c17"
INCLUDE_FLAGS="-I$SRC_DIR/ -I$UTILS_DIR/"

mkdir -p $BUILD_DIR
echo "Building $SRC_DIR"
echo clang $LIB_C_FILESNAMES $C_FILENAMES $COMPILER_FLAGS -o ./$BUILD_DIR/$ASSEMBLY $INCLUDE_FLAGS
clang $LIB_C_FILESNAMES $C_FILENAMES $COMPILER_FLAGS -o ./$BUILD_DIR/$ASSEMBLY $INCLUDE_FLAGS
echo "Build succeded"
echo "Exec location: ./$BUILD_DIR/$ASSEMBLY"
