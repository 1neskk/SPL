.intel_syntax noprefix

.global memcopy
.type memcopy, @function

memcopy:
    mov rax, rdi        # Copy destination address to rax
    mov rcx, rsi        # Copy source address to rcx
    mov r8, rdx         # Copy size to r8

    test r8, r8         # Check if size is zero
    jz .done            # If size is zero, jump to done

.loop:
    mov al, byte ptr [rsi]      # Load byte from source into al
    mov byte ptr [rdi], al      # Store byte from al to destination
    inc rdi                     # Increment destination pointer
    inc rsi                     # Increment source pointer
    dec r8                      # Decrement size
    jnz .loop                   # Repeat until size is zero

.done:
    ret                         # Return from functionn
