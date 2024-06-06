[bits 32]   ; 显式指定为32位汇编

section .text

global enable_paging
global load_page_directory
global get_current_page

; void enable_paging();
enable_paging:
    mov eax, cr0
    or eax, 0x80000000  ; 设置分页启用位（CR0的第31位）
    mov cr0, eax
    ret

; void load_page_directory(uint32_t page_directory_addr);
load_page_directory:
    ; 从栈获取参数
    mov eax, [esp + 4]  ; 从堆栈获取第一个参数，这个计算过程之前讲过
    mov cr3, eax    ; 设置页目录表基地址
    ret

; uint32_t get_current_page(void);
get_current_page:
    mov eax, cr3
    ret
