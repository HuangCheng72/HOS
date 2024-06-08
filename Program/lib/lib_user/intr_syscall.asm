[bits 32]

section .text


; 通用系统调用接口
; 参数通过寄存器传递
; eax - 返回值
; ebx, ecx, edx, esi, edi - 各项参数
global intr_syscall

intr_syscall:
    mov ebx, [esp + 4]      ; 系统调用号
    mov ecx, [esp + 8]      ; 第一个参数
    mov edx, [esp + 12]     ; 第二个参数
    mov esi, [esp + 16]     ; 第三个参数
    mov edi, [esp + 20]     ; 第四个参数
    int 0x80                ; 发出0x80号系统调用中断
    ret                     ; 返回（如果有返回值，都会放在eax寄存器里面，正常读取返回值就行了）
