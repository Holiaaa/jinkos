; JinkOS Project
; Made by Téo JAUFFRET

; This code is the bootloader. It's the first program load by the BIOS when the disk is read.
; It's made to launch jink operating system.

BITS 16
ORG 0x7C00
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

    mov si, errmsg
    call error
    call error

    jmp $

error:
    ; error function
    ; si = link to a string
    mov bl, 0xc
    mov ah, 0xe
    mov al, "E"
    mov bh, 0
    int 0x10
    mov al, "r"
    int 0x10
    mov al, "r"
    int 0x10
    mov al, "o"
    int 0x10
    mov al, "r"
    int 0x10
    mov al, ":"
    int 0x10
    mov al, 0x20
    int 0x10
    mov bl, 0xf
    call print
    ret

print:
    ; print function
    ; si = link to a string
    ; bl = color to print (color are in hexadecimal format)
    mov al, [si]
    inc si
    cmp al, 0
    je .end
    mov ah, 0xe
    mov bh, 0
    int 0x10 
    jmp print
.end:
    ret

set_cursor:
    ; set_cursor function
    ; dl = x
    ; dh = y
    mov ah, 0x02
    mov bh, 0
    int 0x10
    ret

change_video_mode:
    ; change_video_mode function
    ; al = video mode to change with. (http://vitaly_filatov.tripod.com/ng/asm/asm_023.1.html)
    mov ah, 0
    int 0x10
    ret

msg: db "Jink Bootloader", 0
msg2: db "Loading KERNEL.BIN ...", 0
errmsg: db "KERNEL.BIN not found", 0

times 510-($-$$) db 0
dw 0xAA55
