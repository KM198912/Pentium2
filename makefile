CC=i686-elf-gcc
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra -Iinc
LD=$(CC)
LDFLAGS=-T linker.ld -ffreestanding -O2 -nostdlib -lgcc
NASM=nasm
NASMFLAGS=-f elf32
AS=i686-elf-as
ASFLAGS=--32

CFILES=$(shell find . -type f -name '*.c')
ASMFILES=$(shell find . -type f -name '*.asm')
ASFILES=$(shell find . -type f -name '*.s')
OBJFILES=$(CFILES:.c=.o) $(ASMFILES:.asm=.o) $(ASFILES:.s=.o)

all: kernel.bin isofile run

kernel.bin: $(OBJFILES)
	$(LD) $(LDFLAGS) -o $@ $(OBJFILES)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(NASM) $(NASMFLAGS) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f $(OBJFILES) kernel.bin

run: kernel.bin
	qemu-system-i386 -m 2G -cdrom myos.iso -serial stdio	

isofile:
	mkdir -p isodir/boot/grub
	cp kernel.bin isodir/boot/kernel.bin
	echo 'set timeout=0' > isodir/boot/grub/grub.cfg
	echo 'set default=0' >> isodir/boot/grub/grub.cfg
	echo '' >> isodir/boot/grub/grub.cfg
	echo 'menuentry "My Operating System" {' >> isodir/boot/grub/grub.cfg
	echo '	multiboot /boot/kernel.bin' >> isodir/boot/grub/grub.cfg
	echo '	boot' >> isodir/boot/grub/grub.cfg
	echo '}' >> isodir/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso isodir

.PHONY: all clean
