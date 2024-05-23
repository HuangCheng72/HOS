[bits 32]   ; 显式指定为32位汇编

section .text
global store_gdt

store_gdt:
    mov eax, [esp + 4]  ; 获取传入的GDT地址
    sgdt [eax]          ; 存储GDT
    ret
