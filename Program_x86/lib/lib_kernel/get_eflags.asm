[bits 32]

section .text

global get_eflags

; void get_eflags(uint32_t *eflag_var);
get_eflags:
    pushfd              ; 将EFLAGS寄存器的值压入堆栈
    pop eax             ; 将堆栈顶的值弹出到EAX寄存器
    mov ecx, [esp + 4]  ; 获取传入的变量地址，存储到ECX寄存器
    mov [ecx], eax      ; 将EAX寄存器的值存储到变量地址中
    ret                 ; 返回
