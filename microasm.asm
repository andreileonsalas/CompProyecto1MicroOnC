default rel
extern printf,scanf
section .rodata
format2 db "%#x", 10, 0 
SECTION .bss
x resb 4
section .data
format db '%d',0
a  DD 0
b  DD 0
c  DD 0
Temp1  DD 0
Temp2  DD 0
d  DD 0
ww  DD 0
Temp3  DD 0
Temp4  DD 0
section	.text 
   global main
main:;tells linker entry point
mov edi, format
mov esi, x
mov eax, 0
call scanf
 mov [a],eax
mov edi, format
mov esi, x
mov eax, 0
call scanf
 mov [b],eax
mov eax,a
Add eax,b
mov [Temp1],eax
mov eax,Temp1
Sub eax,c
mov [Temp2],eax
mov eax,Temp2
mov [c],eax
mov edi, format
mov esi, x
mov eax, 0
call scanf
 mov [d],eax
mov eax,c
Add eax,d
mov [Temp3],eax
mov eax,Temp3
mov [ww],eax
mov esi, [ww]
lea rdi, [rel format2]
xor eax, eax
call printf
mov eax,ww
Add eax,c
mov [Temp4],eax
mov esi, [Temp4]
lea rdi, [rel format2]
xor eax, eax
call printf

int 80h 
 mov eax,1 
mov ebx,0
int 80h
 
