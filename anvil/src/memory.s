.intel_syntax noprefix

.global memcopy_
.type memcopy_, @function

# rdi = dest, rsi = src, rdx = size
memcopy_:
    test rdx, rdx   # Check if the size is zero
    jz .done

.loop:
    mov bl, byte ptr [rsi]    # Move byte-by-byte from source to rdi (dest)
    mov byte ptr [rdi], bl
    inc rdi
    inc rsi
    dec rdx
    jnz .loop

.done:
    ret
