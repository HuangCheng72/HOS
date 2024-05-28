[bits 32]   ; 显式指定为32位汇编

section .text

global load_page_directory

; void load_page_directory(uint32_t* page_directory);
load_page_directory:
    ; 输入：eax - 页目录表的地址
    ; 从栈获取参数
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]  ; 从堆栈获取第一个参数，这个计算过程之前讲过
    mov cr3, eax    ; 设置页目录表基地址
    pop ebp
    ret
