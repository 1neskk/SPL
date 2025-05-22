.intel_syntax noprefix

.global memcopy_
.global memclear_

.type memcopy_, @function
memcopy_:
    test rdx, rdx   # Check if the size is zero
    jz .done_copy

.loop_copy:
    mov bl, byte ptr [rsi]    # Move byte-by-byte from source to rdi (dest)
    mov byte ptr [rdi], bl
    inc rdi
    inc rsi
    dec rdx
    jnz .loop_copy

.done_copy:
    ret

.type memclear_, @function
memclear_:
    test rsi, rsi
    jz .done_clear

.loop_clear:
    mov byte ptr [rdi], 0     # Clear byte-by-byte
    inc rdi
    dec rsi
    jnz .loop_clear

.done_clear:
    ret
