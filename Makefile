
.PHONY: all clean hd

CC=gcc
CFLAG=-m32 -I include  -c
ASFLAGS=-f elf
BUILD_DIR=build
AS=nasm
LD=ld
LFLAG=-m elf_i386 -Ttext 0xc0001500 -e main

OBJECT= ${BUILD_DIR}/main.o ${BUILD_DIR}/init.o ${BUILD_DIR}/interrupt.o  ${BUILD_DIR}/print.o ${BUILD_DIR}/kernel.o ${BUILD_DIR}/timer.o

${BUILD_DIR}/timer.o: device/timer.c include/print.h include/io.h include/timer.h
	${CC} ${CFLAG} -o $@ $<

${BUILD_DIR}/init.o: kernel/init.c include/print.h include/init.h include/interrupt.h include/timer.h
	${CC} ${CFLAG} -o $@ $<

${BUILD_DIR}/interrupt.o: kernel/interrupt.c include/interrupt.h include/stdint.h include/global.h include/io.h
	${CC} ${CFLAG} -o $@ $<

${BUILD_DIR}/main.o: kernel/main.c include/print.h include/init.h
	${CC} ${CFLAG} -o $@ $<

${BUILD_DIR}/kernel.o: kernel/kernel.S
	${AS} ${ASFLAGS} -o $@ $<

${BUILD_DIR}/print.o: lib/kernel/print.S
	${AS} ${ASFLAGS} -o $@ $<



bin/kernel.bin:${OBJECT}
	${LD} ${LFLAG} $^ -o $@


hd:
	${AS} -I boot/include/  -o bin/loader.bin boot/loader.S
	${AS} -I boot/include/ -o bin/mbr.bin boot/mbr.S
	dd if=bin/mbr.bin of=/myos/bochs/hd60M.img bs=512 count=1 conv=notrunc
	dd if=bin/loader.bin of=/myos/bochs/hd60M.img bs=512 count=4 seek=2 conv=notrunc
	dd if=bin/kernel.bin of=/myos/bochs/hd60M.img bs=512 count=200 seek=9 conv=notrunc 
	

build:bin/kernel.bin

all:build  hd
	

clean:
	rm -rf ${OBJECT} bin/*.bin