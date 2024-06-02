[bits 32]   ; 显式指定为32位汇编

TI_GDT equ 0
RPL0 equ 0
SELECTOR_CODE equ (0x0001 << 3) + TI_GDT + RPL0
SELECTOR_DATA equ (0x0002 << 3) + TI_GDT + RPL0
SELECTOR_VIDEO equ (0x0003 << 3) + TI_GDT + RPL0

section .text

extern interrupt_dispatcher
; 任务切换函数
extern task_switch

%macro interrupt_handler 2
global interrupt_handler_%1

interrupt_handler_%1:
    cli                     ; 禁用中断，防止中断嵌套
    pushad                  ; 保存所有通用寄存器
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

    %if %2
        push dword [esp + 36] ; 错误码在esp+36（错误码，eflags，cs，eip，共16字节，之前压入的8个寄存器共32字节）
    %else
        push 0                ; 没有错误码则压入0
    %endif
    push %1                 ; 压入中断号
    call interrupt_dispatcher ; 调用C里面的中断分发器

    add esp, 8              ; 清理栈上的参数
    pop gs                  ; 恢复各个段寄存器
    pop fs
    pop es
    pop ds
    popad                   ; 恢复所有通用寄存器
    sti                     ; 启用中断

    %if %1 = 32
        call task_switch    ; 只有IRQ0中断（中断号0x20，即32）需要切换任务
    %endif

    iretd                   ; 恢复调用前的cs、eflags、eip
%endmacro

%assign i 0
%rep 48
    %if i = 8 || i = 10 || i = 11 || i = 12 || i = 13 || i = 14 || i = 17
        interrupt_handler i, 1 ; 这些中断有错误码
    %else
        interrupt_handler i, 0 ; 其他中断没有错误码
    %endif
    %assign i i+1
%endrep
