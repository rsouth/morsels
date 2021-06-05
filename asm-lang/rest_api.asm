
bits 64
section .data
    ; null-terminated 'Hello World' and it's length
    ; db defines a byte
    ; see https://www.tutorialspoint.com/assembly_programming/assembly_variables.htm
    ; equ == 'equate' == #define :) no allocation just defining label
    msg                 db      'Who are you?',0xa
    len                 equ     $-msg

	hi_msg	           db       'Hi there, '
	len_hi            equ      $-hi_msg

section .bss
	name	resb    5

section .text
    global _start

    	; == write to stdout ========================
	_print:
		mov ebx,1
		mov eax,4
		syscall
		ret


    ; == _start - entry point ==========
    _start:
                                ; write 'Hello World' to stdout
        mov rax,1   ; opcode for sys_write
		mov rdi,1   ; fd for stdout
		mov rsi,msg ; string to output
		mov rdx,len
        syscall

		; read input
		mov rax,0    ; opcode is 0 for sys_read
		mov rdi,0    ; fd is 0
		mov rsi,name ; buffer
		mov rdx,5    ; buf size
		syscall

		mov rax,1
		mov rdi,1
		mov rsi,hi_msg
		mov rdx,len_hi
		syscall

		mov rax,1
		mov rdi,1
		mov rsi,name
		mov rdx,5
		syscall






_socket_setup:
	; == init vars ==============================

	; == socket call ============================
	; %rax 	Name 	Entry point 	Implementation
	; 41	socket	sys_socket	net/socket.c

	; %rdi			%rsi		%rdx
	; int family	int type	int protocol
	; AF_INET = 2: https://github.com/torvalds/linux/blob/master/include/linux/socket.h
	mov rax,41		; socket()
	mov rdi,2 		; family (2 == AF_INET / PF_INET)
	mov rsi,1		; type (1==TCP https://github.com/torvalds/linux/blob/master/include/linux/socket.h)
	mov rdx,0	    ; protocol (0 == automatically determine)
	syscall

	; == bind call ==============================
	; %rax 	Name 	Entry point 	Implementation
	; 49	bind	sys_bind	net/socket.c

	; %rdi		%rsi								%rdx
	; int fd	struct sockaddr __user * umyaddr	int addrlen

	mov r10,rax
	mov rax,49		; bind()
	pop rdi			; set rdi to fd
	mov rsi,0x1152	; port 41769
	mov rdx,16		; addrlen
	syscall

	; push rax		; store fd

	; == listen call ============================
	; %rax 	Name 	Entry point 	Implementation
	; 50	listen	sys_listen	net/socket.c
	; %rdi		%rsi
	; int fd	int backlog

	mov rax,50       ; listen
	mov rdi,r10			 ; rdi == fd to flag for listening
	; mov r10,rdi
	mov rsi,5        ; 5 conns in backlog
	syscall


	; == accept call ============================
; %rax 	Name 	Entry point 	Implementation
; 43	accept	sys_accept	net/socket.c
; %rdi		%rsi									%rdx
; int fd	struct sockaddr __user * upeer_sockaddr	int __user * upeer_addrlen

	

	mov rax, 43
	mov rdi,r10
	mov rsi,0x0
	mov rdx,0x0

	syscall


                                ; quit the program
        mov	rax,60               ; system call for sys_exit
		mov rdi,0
        syscall