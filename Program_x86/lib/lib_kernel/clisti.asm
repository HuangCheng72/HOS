; 就两个中断操作函数

[bits 32]

section .text

global disable_interrupts
global enable_interrupts

; void disable_interrupts();
disable_interrupts:
    cli
    ret

; void enable_interrupts();
enable_interrupts:
    sti
    ret
