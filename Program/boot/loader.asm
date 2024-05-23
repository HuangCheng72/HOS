;------------------- loader.inc -------------------

%include "boot.inc"

;------------------- loader代码段 -------------------
section loader vstart=LOADER_BASE_ADDR

; 补充说明，栈的生长是向低位生长，所以栈顶指针设在高位并不会导致栈的数据覆盖Loader，因为二者写的方向都不一样
LOADER_STACK_TOP equ LOADER_BASE_ADDR

; jmp 指令在 x86 汇编中有多种形式，常见的形式有：
; jmp short：2 字节，距离范围为 -128 到 +127 字节。（jmp本身1字节，距离1字节）
; jmp near： 3 字节，距离范围为 -32768 到 +32767 字节。（jmp本身1字节，距离2字节）
; jmp far：  5 字节，用于跳转到绝对地址，通常是跨段跳转，即改变段寄存器的值。
; jmp far 不仅需要改变 eip，还需要改变段选择子，因此需要 4 字节的目标地址（段选择子 + 偏移量）。
; 编译器会自动计算你的距离，从而决定要用哪个，你也可以强制指定

; 在这里很显然我们的jmp是near，3字节大小
jmp loader_start
; 用jmp的原因是因为我不知道为什么下面的GDT会被当成代码执行，人都麻了

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

; 首先，equ指令相当于C的define，就是简单的替换，编译的时候就会清理掉，不占空间
; jmp刚刚说了是3字节
; 很明显GDT位置从0x903开始起算
; GDT之前计算过了 256 字节
; 起始地址是0x900，对应十进制数字是2304，2304 + 3 + 256 = 2563，对应16进制是0xa03
; 也就是说0xa02是gdt空位结束，gdt_ptr的起始地址是下一个字节0xa03，读0xa03指向的值，就知道GDT指针信息

; 定义GDT指针，前2字节是GDT界限，后4字节是GDT起始地址
gdt_ptr:
    dw GDT_LIMIT
    dd GDT_BASE


; gdt_ptr是一个dw和一个dd，dw是2字节，对应C语言的unsigned char，dw是4字节，对应C语言里的unsigned int，一共是6字节空间
; 2563 + 6 = 2569，十六进制为0xa09
; total_mem_bytes的起始地址就是0xa09
total_mem_bytes dd 0

loader_start:
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

    call get_memory
    mov byte [gs:160], 'P'  ; 在屏幕位置(0,10)处写入字符'P'
    
    jmp KERNEL_BASE_ADDR


; 《30天自制操作系统》里面的内存容量获取方式，完全不用BIOS，但是必须进入32位之后才能执行
[bits 32]
get_memory:
    ; 保存当前的寄存器值
    push edi                     ; 保存 edi 寄存器的值到堆栈
    push esi                     ; 保存 esi 寄存器的值到堆栈
    push ebx                     ; 保存 ebx 寄存器的值到堆栈

    ; 设置测试模式的两个模式字
    mov esi, 0xaa55aa55          ; 将第一个模式字 0xaa55aa55 存入 esi
    mov edi, 0x55aa55aa          ; 将第二个模式字 0x55aa55aa 存入 edi

    ; 开始地址设置为1MB，前1MB也就是0到0x000fffff分配给MBR、Loader、Kernel
    ; 内存小于1MB建议直接写裸机算了要什么操作系统
    mov eax, 0x00100000

.get_memory_loop:
    ; 计算当前测试的内存地址
    mov ebx, eax                 ; 将当前地址复制到 ebx
    add ebx, 0xffc               ; 将 ebx 增加 0xffc（测试 4KB 内存块的最后 4 字节）

    ; 保存当前内存值，并写入测试模式值
    mov edx, [ebx]               ; 将当前内存值存入 edx
    mov [ebx], esi               ; 将第一个模式字写入内存
    xor dword [ebx], 0xffffffff  ; 将内存中的值进行按位取反

    ; 检查是否匹配第一个模式
    cmp edi, [ebx]               ; 将内存值与第二个模式字进行比较
    jne .get_memory_fail         ; 如果不匹配，则跳转到失败处理
    xor dword [ebx], 0xffffffff  ; 如果匹配，则恢复到原始模式字

    ; 检查是否匹配第二个模式
    cmp esi, [ebx]               ; 将内存值与第一个模式字进行比较
    jne .get_memory_fail         ; 如果不匹配，则跳转到失败处理

    ; 恢复原始内存值，并继续测试下一个地址
    mov [ebx], edx               ; 将原始内存值恢复到内存
    add eax, 0x00001000          ; 将地址增加 4KB
    cmp eax, 0x0fffffff          ; 将当前地址与结束地址比较，结束地址设置为4G - 1，也就是0xffffffff
    jbe .get_memory_loop         ; 如果当前地址小于或等于结束地址，则继续测试下一个内存块

    ; 测试成功，恢复寄存器并返回
    pop ebx                      ; 恢复 ebx 寄存器的值
    pop esi                      ; 恢复 esi 寄存器的值
    pop edi                      ; 恢复 edi 寄存器的值
    mov [total_mem_bytes], eax	 ; 将最终的内存容量保存到total_mem_bytes中
    clc                          ; 清除错误标志，表示内存测试成功
    ret                          ; 返回调用者

.get_memory_fail:
    ; 测试失败，保存当前地址到total_mem_bytes，并恢复原始内存值
    mov [total_mem_bytes], eax   ; 将当前地址保存到total_mem_bytes
    pop ebx                      ; 恢复 ebx 寄存器的值
    pop esi                      ; 恢复 esi 寄存器的值
    pop edi                      ; 恢复 edi 寄存器的值
    stc                          ; 设置错误标志，表示内存测试失败
    ret                          ; 返回调用者
