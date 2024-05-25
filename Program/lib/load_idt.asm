[bits 32]   ; 显式指定为32位汇编

section .text
global load_idt

; void load_idt(uint16_t limit, uint32_t base);
load_idt:
    ; 从栈中获取limit和base
    mov ax, [esp + 4]   ; 获取limit低16位
    mov word [esp + 4], ax
    mov eax, [esp + 8]  ; 获取base
    mov [esp + 6], eax
    lidt [esp + 4]      ; 加载GDT
    ret

