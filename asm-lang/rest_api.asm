
bits 64

section .data
    msg             db      'No-REST API',0xa
    len             equ     $-msg

    s_connected     db      'Client Connected',0xa
    l_connected     equ     $-s_connected

    s_goodbye       db      '🍒',0xa
    l_goodbye       equ     $-s_goodbye

section .bss
    name    resb    5
    buffer  resb    255     ; variable to store request headers

section .text
    global _start

; == write to stdout ============================
_print:
    mov rdi,1           ; fd for stdout
    mov rax,1           ; opcode for sys_write
    syscall
    ret

; == read from stdin ============================
_read_input:
    mov rax,0           ; opcode is 0 for sys_read
    mov rdi,0           ; fd is 0
    mov rsi,name        ; buffer
    mov rdx,5           ; buf size
    syscall

; == quit =======================================
_quit:
    ; TODO allow caller to set rdi
    mov rax,60          ; sys_exit
    mov rdi,0
    syscall

; == _start - entry point =======================
_start:
    ; hello world :)
    mov rsi,msg
    mov rdx,len
    call _print

; == init vars ==================================
; TODO should I be xor-ing the registers first?

; == create socket ==============================
_socket:
    ; %rax  Name    Entry point     Implementation
    ; 41    socket  sys_socket      net/socket.c

    ; %rdi          %rsi        %rdx
    ; int family    int type    int protocol

    ; AF_INET = 2 (https://github.com/torvalds/linux/blob/master/include/linux/socket.h)
    mov rax,41          ; socket()
    mov rdi,2           ; family (2 == AF_INET / PF_INET)
    mov rsi,1           ; type (1==TCP https://github.com/torvalds/linux/blob/master/include/linux/socket.h)
    mov rdx,0           ; protocol (0 == automatically determine)
    syscall

; == bind call ==================================
_bind:
    ; %rax  Name    Entry point     Implementation
    ; 49    bind    sys_bind        net/socket.c

    ; %rdi      %rsi                                %rdx
    ; int fd    struct sockaddr __user * umyaddr    int addrlen

    mov r10,rax
    mov rax,49          ; bind()
    pop rdi             ; set rdi to fd
    mov rsi,0x1152      ; port 41295 todo: fix this.
    mov rdx,16          ; addrlen
    syscall

    ; push rax      ; store fd

; == listen call ================================
_listen:
    ; %rax  Name    Entry point     Implementation
    ; 50    listen  sys_listen      net/socket.c

    ; %rdi      %rsi
    ; int fd    int backlog

    mov rax,50          ; listen
    mov rdi,r10         ; rdi == fd to flag for listening
    ; mov r10,rdi
    mov rsi,5           ; 5 conns in backlog
    syscall

; == accept call ================================
_accept:
    ; %rax  Name    Entry point     Implementation
    ; 43    accept  sys_accept      net/socket.c

    ; %rdi      %rsi                                    %rdx
    ; int fd    struct sockaddr __user * upeer_sockaddr int __user * upeer_addrlen

    mov rax,43
    mov rdi,r10         ; set rdi to the fd
    xor rsi,rsi         ; clear rsi and rdx
    xor rdx,rdx
    syscall

; == fork() -> handle client ====================
_fork:
    ; https://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/
    mov rax,57
    mov rsi,rax         ; store the socket fd (or -1 for error)
    syscall

    cmp rax, 0          ; pid == 0 for child, child's pid for parent
    jz  _recvmsg        ; child: start recv()-ing
    jmp _accept         ; parent: accept more connections

; == recv =======================================
_recvmsg:
    ; %rax    Name    Entry point     Implementation
    ; 47      recvmsg sys_recvmsg     net/socket.c

    ; %rdi      %rsi                        %rdx
    ; int fd    struct msghdr __user * msg  unsigned int flags

    mov rdi,rax
    mov rax,47
    mov rsi,buffer
    mov rdx,0
    syscall

    mov rsi,buffer
    mov rdx,255
    call _print

;     mov     rdx, 255            ; buffer and buffer size
;     mov     rcx, buffer         ; ...


; == done.... ===================================
    mov rsi,s_goodbye
    mov rdx,l_goodbye
    call _print

    call _quit
