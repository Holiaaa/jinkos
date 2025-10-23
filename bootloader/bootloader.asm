; JinkOS Project
; Made by Téo JAUFFRET

; This code is the bootloader. It's the first program load by the BIOS when the disk is read.
; It's made to launch jink operating system.

BITS 16
ORG 0x7e00

global start
start:
    mov al, 0x10
    call change_video_mode

    mov dh, 10
    mov dl, 33
    call set_cursor

    mov si, msg
    mov bl, 0x9
    call print

    mov dh, 12
    mov dl, 30
    call set_cursor

    mov si, msg2
    mov bl, 0xb
    call print

    mov dh, 24
    mov dl, 26
    call set_cursor

    ;mov si, errmsg
    ;call error

    jmp $

%INCLUDE "boot_utils.inc"

msg: db "Jink Bootloader", 0
msg2: db "Loading KERNEL.BIN ...", 0
errmsg: db "KERNEL.BIN not found", 0

times 1000 db 0
