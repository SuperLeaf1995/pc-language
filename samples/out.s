[section .text]

_hello_world:
mov ah, 0eh
mov al, 'h'
int 10h
mov al, 'e'
int 10h
mov al, 'l'
int 10h
mov al, 'l'
int 10h
mov al, 'o'
int 10h
mov al, ' '
int 10h
mov al, 'w'
int 10h
mov al, 'o'
int 10h
mov al, 'r'
int 10h
mov al, 'l'
int 10h
mov al, 'd'
int 10h
jmp $
