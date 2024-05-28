[bits 32]   ; 显式指定为32位汇编

section .text

global enable_paging

; void enable_paging();
enable_paging:
    push ebp
    mov ebp, esp
    mov eax, cr0
    or eax, 0x80000000  ; 设置分页启用位（CR0的第31位）
    mov cr0, eax
    pop ebp
    ret
