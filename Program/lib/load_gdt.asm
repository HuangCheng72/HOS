[bits 32]   ; 显式指定为32位汇编

section .text
global load_gdt

load_gdt:
    mov eax, [esp + 4]  ; 获取传入的GDT地址
    lgdt [eax]          ; 加载GDT
    ret
