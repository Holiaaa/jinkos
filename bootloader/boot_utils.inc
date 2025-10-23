; JinkOS Project
; Made by Téo JAUFFRET

; This file contains essential functions for the bootloader.

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
