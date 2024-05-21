;------------------- loader.inc -------------------

%include "boot.inc"

;------------------- loader代码段 -------------------
section loader vstart=LOADER_BASE_ADDR
LOADER_STACK_TOP equ LOADER_BASE_ADDR

jmp loader_start  ; 跳转到loader_start处开始执行代码

;------------------- 构建GDT及其内部的描述符 -------------------
GDT_BASE:   
    dd 0x00000000  ; 第一个描述符（空描述符）
    dd 0x00000000

CODE_DESC:  
    dd 0x0000FFFF  ; 代码段基址低16位和段限制低16位
    dd DESC_CODE_HIGH4  ; 代码段的高32位描述符属性

DATA_STACK_DESC:  
    dd 0x0000FFFF  ; 数据段基址低16位和段限制低16位
    dd DESC_DATA_HIGH4  ; 数据段的高32位描述符属性

VIDEO_DESC: 
    dd 0x80000007  ; 视频段基址低16位和段限制低16位 (limit=(0xbffff-0xb8000)/4k=0x7)
    dd DESC_VIDEO_HIGH4  ; 视频段的高32位描述符属性，此时DPL已改为0

GDT_SIZE equ $ - GDT_BASE
GDT_LIMIT equ GDT_SIZE - 1 
times 28 dq 0  ; 预留28个描述符空位，加上已有的4个，一共是32个描述符，每个描述符8个字节，总共是32 * 8 = 256字节

; 选择子
SELECTOR_CODE equ (0x0001 << 3) + TI_GDT + RPL0  ; 代码段选择子
SELECTOR_DATA equ (0x0002 << 3) + TI_GDT + RPL0  ; 数据段选择子
SELECTOR_VIDEO equ (0x0003 << 3) + TI_GDT + RPL0  ; 视频段选择子
    
; 定义GDT指针，前2字节是GDT界限，后4字节是GDT起始地址
gdt_ptr:  
    dw GDT_LIMIT
    dd GDT_BASE


loadermsg db '2 loader in real.'

loader_start:

;------------------------------------------------------------
; INT 0x10    功能号:0x13    功能描述:打印字符串
;------------------------------------------------------------
; 输入:
; AH 子功能号=13H
; BH = 页码
; BL = 属性(若AL=00H或01H)
; CX = 字符串长度
; (DH、DL) = 坐标(行、列)
; ES:BP = 字符串地址 
; AL = 显示输出方式
;    0 —— 字符串中只含显示字符，其显示属性在BL中。显示后，光标位置不变
;    1 —— 字符串中只含显示字符，其显示属性在BL中。显示后，光标位置改变
;    2 —— 字符串中含显示字符和显示属性。显示后，光标位置不变
;    3 —— 字符串中含显示字符和显示属性。显示后，光标位置改变
; 无返回值
    mov sp, LOADER_BASE_ADDR  ; 设置栈顶地址
    mov bp, loadermsg  ; 设置BP指向字符串地址
    mov cx, 17  ; 设置字符串长度为17
    mov ax, 0x1301  ; 设置AX为0x1301，AH=0x13 (功能号)，AL=0x01 (显示方式)
    mov bx, 0x001f  ; 设置蓝底粉红字，页号为0 (BH=0)，属性为0x1f (BL=0x1f)
    mov dx, 0x1800  ; 设置显示位置（行=0x18，列=0x00）
    int 0x10  ; 调用BIOS中断0x10打印字符串
    
    jmp enable_p_mode  ; 开启保护模式


enable_p_mode:
;----------------------------------------   开启保护模式   ------------------------------------------
; 1. 打开A20
; 2. 加载GDT
; 3. 将CR0的PE位的值置为1

;-----------------  打开A20  ----------------
    ; 打开A20的方法有好几种，有直接用BIOS中断打开的，还有用键盘控制器打开的
    ; 《30天自制操作系统》里面就是用键盘控制器打开的
    in al,0x92  ; 从端口0x92读取当前值到AL
    or al,0000_0010B  ; 设置A20启用位（第1位）
    out 0x92,al  ; 写回到端口0x92

;-----------------  加载GDT  ----------------
    lgdt [gdt_ptr]  ; 加载GDT指针到GDTR寄存器

;-----------------  将CR0第0位置1  ----------------
    cli            ; 关闭中断
    mov eax, cr0  ; 读取CR0寄存器的值到EAX
    or eax, 0x00000001  ; 设置CR0的PE位（第0位）
    mov cr0, eax  ; 写回CR0寄存器，启用保护模式

    jmp SELECTOR_CODE:p_mode_start  ; 远跳转到保护模式代码段，刷新流水线，避免分支预测的影响


[bits 32]  ; 进入32位模式
p_mode_start:
    mov ax, SELECTOR_DATA  ; 加载数据段选择子到AX
    mov ds, ax  ; 设置DS段寄存器
    mov es, ax  ; 设置ES段寄存器
    mov ss, ax  ; 设置SS段寄存器
    mov esp, LOADER_STACK_TOP  ; 设置ESP为栈顶地址
    mov ax, SELECTOR_VIDEO  ; 加载视频段选择子到AX
    mov gs, ax  ; 设置GS段寄存器

    mov byte [gs:160], 'P'  ; 在屏幕位置(0,10)处写入字符'P'
    
    hlt  ; 停止CPU
    jmp $  ; 无限循环，停在此处
