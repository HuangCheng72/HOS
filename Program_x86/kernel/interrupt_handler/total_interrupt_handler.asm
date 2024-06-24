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

; 定义0x80号系统调用中断处理程序
global syscall_handler

; C里面写的系统调用分发器（见syscall_dispatcher.c）
extern syscall_dispatcher

syscall_handler:
    ; 防止嵌套中断，要关闭中断
    cli

    pushad
    push ds
    push es
    push fs
    push gs
    ; 把这几个置换为内核态的段选择子
    mov ax, SELECTOR_DATA
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ax, SELECTOR_VIDEO
    mov gs, ax

    ; ---------    老规矩，中断开始处理前保存上下文，重新加载段选择子    ---------

    ; --------------- 这里是配合系统调用分发器的设计 ---------------

    push edi                ; 第四个参数
    push esi                ; 第三个参数
    push edx                ; 第二个参数
    push ecx                ; 第一个参数
    push ebx                ; 系统调用号

    call syscall_dispatcher ; 调用C里的系统调用分发器

    add esp, 20             ; 清理栈上的参数（五个，一共是20字节）

    ; 将返回值存到栈上先前保存eax的位置，这样等下就可以弹栈弹回来了
    ; 8个通用寄存器和4个段寄存器，一共12个都是4字节
    ; eax是第一个保存的（所以偏移11 * 4，它的低地址有11个寄存器的值，mov的方向是从低地址到高地址）
    mov [esp + 11 * 4], eax

   ; ---------    老规矩，中断处理完毕恢复上下文    ---------
    pop gs
    pop fs
    pop es
    pop ds
    popad

    sti

    ; 数据是CPU自动帮我们压栈的，直接iretd弹回去就是了
    ; 回到用户态
    iretd
