.globl memcopy
.type memcopy, @function
memcopy:
    movq %rdi, %rax        # Copy destination address to rax
    movq %rsi, %rcx        # Copy source address to rcx
    movq %rdx, %r8         # Copy size to r8

    testq %r8, %r8         # Check if size is zero
    jz .done               # If size is zero, jump to done

.loop:
    movb (%rcx), %al       # Load byte from source
    movb %al, (%rax)       # Store byte to destination
    incq %rax              # Increment destination pointer
    incq %rcx              # Increment source pointer
    decq %r8               # Decrement size
    jnz .loop              # Repeat until size is zero

.done:
    ret                    # Return from function
