start:
    mov bx, 0x1000
    mov [bx], 'H'
    mov [bx+1], 'e'
    mov [bx+2], 'l'
    mov [bx+3], 'l'
    mov [bx+4], 'o'
    mov [bx+5], 0x20
    mov [bx+6], 'W'
    mov [bx+7], 'o'
    mov [bx+8], 'r'
    mov [bx+9], 'l'
    mov [bx+10], 'd'
    mov [bx+11], '!'
    mov [bx+12], 0

    mov cx, 0x1000
    out cx, 13
    halt