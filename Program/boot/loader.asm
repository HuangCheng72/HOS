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
    ; 在进入保护模式之前先读内核
    ; 读入Kernel，120个扇区足够大了


    ; BIOS 0x13中断0x02功能的参数列表

    ; AH: 设置为0x02，表示读取扇区功能。
    ; AL: 要读取的扇区数（必须在1到128之间）。
    ; CH: 柱面号（0-1023，标准CHS模式）。
    ; CL: 扇区号（1-63）| ((柱面号 >> 2) & 0xC0)。
    ; DH: 磁头号（0-255）。
    ; DL: 驱动器号（通常第一个硬盘为0x80）。
    ; ES:BX: 数据缓冲区的段:偏移指针，用于存储读取的数据。

    ; 16位下寻址方式是段 * 16 + 偏移量。所以是 es * 16 + bx。
    ; 起始地址 0x1500（主要看KERNEL_BASE_ADDR） ，其段就是0x150，偏移量就是00

    ; 初始化目标地址段
    mov ax, KERNEL_BASE_ADDR
    shr ax, 4
    mov es, ax
    xor ax, ax

    mov ax, KERNEL_START_SECTOR ; 读取起始扇区号 (LBA)

    ; 计算LBA扇区号到CHS各项参数
    ; 计算步骤:
    ; 1. 扇区号 (Sector)   = (LBA 扇区号 % 每磁头扇区数) + 1
    ; 2. 磁头号 (Head)     = (LBA 扇区号 / 每磁头扇区数) % 磁头数每柱面
    ; 3. 柱面号 (Cylinder) = (LBA 扇区号 / 每磁头扇区数) / 磁头数每柱面

    ; 建立硬盘文件的参数，共计130个柱面，每个柱面16个磁头，每个磁头63个扇区
    ; 每个柱面总计63 * 16 = 1008 个扇区

    ; 现在是16位实模式，用16位除法，也就是dx:ax 组合成一个32位的被除数，除法运算之后，商在 ax，余数在 dx。

    ; 清空bx，用来存放计算结果
    xor bx, bx

    ; 计算扇区号、磁头号（用低位就行了，因为真没那么多）
    xor dx, dx                  ; 清除高16位为0
    mov di, 63                  ; 每磁头有63个扇区
    div di                      ; 商: ax (磁头号), 余数: dx (扇区号)
    mov bl, dl                  ; 扇区号 (0-62)
    add bl, 1                   ; 扇区号从1开始
    mov bh, al                  ; 磁头号 (0-15)

    ; 本来还要计算柱面号，但是也每那么多，直接写死算了，柱面号肯定是0，根本没那么多扇区
    ; 所以cx组合起来，只需要解决扇区号的问题就行了
    xor dx, dx                  ; 清除数据
    mov dh, bh                  ; 转移磁头号信息
    ; 前面也说了柱面号的问题，柱面号肯定是0，所以清空ch
    xor ch, ch
    ; cl的问题只要保证高2位是0就行了
    and bl, 0x3f                ; 清除 bl 的高 2 位，保留原 bl 的低 6 位 (扇区号)
    or cl, bl                   ; 位运算直接赋值

    xor ax, ax

    ; 设置BIOS中断0x13的参数
    mov al, 50                  ; 读取50个扇区
    mov ah, 0x02                ; 读扇区命令
    mov dl, 0x80                ; 驱动器号，第一个硬盘

    ; 设置开始的时候是16进制下的整数，偏移量就永远是0
    xor bx, bx
    int 0x13                    ; 调用BIOS中断
    ; 检查是否成功
    jc error                    ; 如果出错，跳转到错误处理

    jmp enable_p_mode           ; 进行后续设置

error:
    jmp $                       ; 错误处理，死循环


enable_p_mode:
    ; 不屏蔽中断，进不去QEMU，会无限重启，本来要屏蔽中断就是一个必备流程，bochs里面能用不代表qemu或者真机能用
    cli
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
    ;mov byte [gs:160], 'P'  ; 在屏幕位置(0,10)处写入字符'P'
    
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
