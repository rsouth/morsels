; hello_world


section .data
    msg       db     'Hello world!'
    nl        db     0x0a
    msgLen    equ    $-msg
section .text
global _start
_start:
    mov rax, 1
    mov rdi, 1
    mov rsi, msg
    mov rdx, msgLen
    syscall

    mov rax, 60
    mov rdi, 0
    syscall
