[bits 32]   ; 显式指定为32位汇编

TI_GDT equ 0
RPL0 equ 0
SELECTOR_CODE equ (0x0001 << 3) + TI_GDT + RPL0
SELECTOR_DATA equ (0x0002 << 3) + TI_GDT + RPL0
SELECTOR_VIDEO equ (0x0003 << 3) + TI_GDT + RPL0

section .text

global interrupt_handler_0x20

; 在C里面编写的逻辑处理代码，叫什么都可以
; 没有的话就注释掉
;extern interrupt_0x20_function

; void interrupt_handler_0x20();
interrupt_handler_0x20:
    cli                     ; 禁用中断，防止中断嵌套
    pushad                  ; 保存所有通用寄存器，也就是所谓的保存现场
    push ds                 ; 保存数据段寄存器
    push es
    push fs
    push gs                 ; 保存视频段寄存器
    mov ax, SELECTOR_DATA   ; 加载数据段选择子
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ax, SELECTOR_VIDEO  ; 加载视频段选择子
    mov gs, ax

    ;call interrupt_0x20_function ; 调用C里面的处理代码

    pop gs                  ; 恢复各个段寄存器
    pop fs
    pop es
    pop ds
    popad                   ; 恢复所有通用寄存器，也就是恢复现场
    sti                     ; 启用中断
    iretd                   ; 恢复调用前的cs、eflags、eip
