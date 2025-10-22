:: JinkOS Project
:: Made by Téo JAUFFRET

:: build script for building the os in a floopy disk and running it with qemu 

@echo off
nasm -f bin bootloader/entry.asm -o bootloader/entry.bin
nasm -f bin bootloader/bootloader.asm -o bootloader/bootloader.bin

if exist bootloader/jink.flp del /F /q bootloader/jink.flp
fsutil file createnew bootloader/jink.flp 1474560

dd if=bootloader/entry.bin of=bootloader/jink.flp bs=512 conv=notrunc
dd if=bootloader/bootloader.bin of=bootloader/jink.flp bs=512 seek=1 conv=notrunc

qemu-system-i386 -fda bootloader/jink.flp
