
.PHONY: all clean
all:
	nasm boot/loader.S -I boot/include/ -o boot/loader.bin
	nasm boot/mbr.S -I boot/include/ -o boot/mbr.bin
	nasm -f elf -o lib/kernel/print.o lib/kernel/print.S
	gcc -m32 -I kernel/include/ -c -o kernel/main.o  kernel/main.c 
	ld -m elf_i386 kernel/main.o lib/kernel/print.o -Ttext 0xc0001500 -e main -o kernel/kernel.bin 
	dd if=kernel/kernel.bin of=/myos/bochs/hd60M.img bs=512 count=200 seek=9 conv=notrunc 
	dd if=boot/mbr.bin of=/myos/bochs/hd60M.img bs=512 count=1 conv=notrunc
	dd if=boot/loader.bin of=/myos/bochs/hd60M.img bs=512 count=4 seek=2 conv=notrunc
clean:
	rm -rf boot/loader.bin boot/mbr.bin kernel/main.o lib/kernel/print.o
