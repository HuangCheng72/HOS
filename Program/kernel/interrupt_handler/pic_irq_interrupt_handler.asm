[bits 32]   ; 显式指定为32位汇编

TI_GDT equ 0
RPL0 equ 0
SELECTOR_CODE equ (0x0001 << 3) + TI_GDT + RPL0
SELECTOR_DATA equ (0x0002 << 3) + TI_GDT + RPL0
SELECTOR_VIDEO equ (0x0003 << 3) + TI_GDT + RPL0

section .text

global pic_irq_interrupt_handler

; 在C里面编写的逻辑处理代码，现在是中断分发器
extern pic_irq_interrupt_dispatcher

; void pic_irq_interrupt_handler();
pic_irq_interrupt_handler:
    cli                     ; 禁用中断，防止中断嵌套（eflag、cs、eip、错误码，四个，一共16字节）
    pushad                  ; 保存所有通用寄存器，也就是所谓的保存现场（8个寄存器，每个4字节，一共32字节）
    push ds                 ; 保存数据段寄存器（段寄存器一共四个，每个4字节，一共16字节）
    push es
    push fs
    push gs                 ; 保存视频段寄存器
    mov ax, SELECTOR_DATA   ; 加载数据段选择子
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ax, SELECTOR_VIDEO  ; 加载视频段选择子
    mov gs, ax

    call pic_irq_interrupt_dispatcher ; 调用C里面的中断分发器

    pop gs                  ; 恢复各个段寄存器
    pop fs
    pop es
    pop ds
    popad                   ; 恢复所有通用寄存器，也就是恢复现场
    sti                     ; 启用中断
    iretd                   ; 恢复调用前的cs、eflags、eip
