CC=gcc
ASMBIN=nasm

all : asm cc link
asm : 
	$(ASMBIN) -o rotate_8x8.o -f elf64 rotate_8x8.asm
cc :
	$(CC) -c -g -O0 project.c
link :
	$(CC) -o project project.o rotate_8x8.o
clean :
	rm *.o
	rm project
