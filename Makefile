
PORT := $$(date +'%s')
PORT := $(shell echo "$$(( ( $(PORT) & 4095 ) + 1024))" )
current_dir=$(shell pwd)


all: hello rootfs.img 
	mcopy -i rootfs.img@@1M hello ::/
	mmd -i rootfs.img@@1M boot 
	mcopy -i rootfs.img@@1M grub.cfg ::/boot
#	mcopy -i rootfs.img@@1M test.txt ::/

hello: $(current_dir)/main/hello.c
	gcc -c -ffreestanding -mgeneral-regs-only -mno-mmx -m32 -march=i386 -fno-pie -fno-stack-protector -g3 -Wall $(current_dir)/main/hello.c $(current_dir)/main/page.c $(current_dir)/main/fat.c $(current_dir)/main/putc.c $(current_dir)/main/rprintf.c  
	nasm -f elf32 -g -o ide.o $(current_dir)/main/ide.s
	ld -T"kernel.ld" -e main -melf_i386  hello.o page.o fat.o ide.o putc.o rprintf.o -o hello 


rootfs.img:
	dd if=/dev/zero of=rootfs.img bs=1M count=32
	grub-mkimage -p "(hd0,msdos1)/boot" -o grub.img -O i386-pc normal biosdisk multiboot multiboot2 configfile fat exfat part_msdos
	dd if=/usr/lib/grub/i386-pc/boot.img  of=rootfs.img conv=notrunc
	dd if=grub.img of=rootfs.img conv=notrunc seek=1
	echo 'start=2048, type=83, bootable' | sfdisk rootfs.img
	mkfs.fat --offset 2048 -F16 rootfs.img

debug:
#        screen -S qemu -d -m qemu-system-i386 -curses -s -drive file=rootfs.img,format=raw -monitor stdio
#	screen -S qemu -d -m qemu-system-i386 -curses -s -drive file=rootfs.img,format=raw
	screen -S qemu -d -m qemu-system-i386 -S -s -hda rootfs.img -monitor stdio
	TERM=xterm gdb -x gdb_os.txt && killall -show-cursor qemu-system-i386



	

clean:
	rm -f grub.img hello hello.o putc.o rprintf.o page.o ide.o fat.o rootfs.img
