; 视频段选择子就是为了用于找光标所在位置
TI_GDT equ 0
RPL0 equ 0
SELECTOR_VIDEO equ (0x0003 << 3) + TI_GDT + RPL0

section .data
put_int_buffer dq 0     ; 定义8字节缓冲区用于数字到字符的转换（这个要放到数据段，因为是个占据内存的变量）

[bits 32]
section .text

global put_char
global put_str
global put_int

;------------------------   put_char   -----------------------------
; 功能描述: 把栈中的1个字符写入光标所在处
; C声明原型：void put_char(char c);
;-------------------------------------------------------------------
put_char:
    pushad                     ; 备份所有32位寄存器
    mov ax, SELECTOR_VIDEO     ; 设置视频段选择子
    mov gs, ax

    ; 获取当前光标位置高8位
    mov dx, 0x03d4
    mov al, 0x0e
    out dx, al
    mov dx, 0x03d5
    in al, dx
    mov ah, al

    ; 获取当前光标位置低8位
    mov dx, 0x03d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x03d5
    in al, dx

    ; 将光标位置存入bx
    mov bx, ax
    mov ecx, [esp + 36]        ; 获取待打印的字符
    cmp cl, 0xd                ; 检查是否为回车符CR (0x0d)
    jz .is_carriage_return
    cmp cl, 0xa                ; 检查是否为换行符LF (0x0a)
    jz .is_line_feed
    cmp cl, 0x8                ; 检查是否为退格符BS (0x08)
    jz .is_backspace
    jmp .put_other

.is_backspace:
    ; 处理退格符，删除光标前一个字符
    dec bx
    shl bx, 1
    mov byte [gs:bx], 0x20     ; 用空格覆盖退格位置字符
    inc bx
    mov byte [gs:bx], 0x07
    shr bx, 1
    jmp .set_cursor

.put_other:
    ; 打印其他字符
    shl bx, 1
    mov [gs:bx], cl            ; 打印字符
    inc bx
    mov byte [gs:bx], 0x07     ; 设置字符属性
    shr bx, 1
    inc bx
    cmp bx, 2000
    jl .set_cursor             ; 如果光标未超出屏幕范围，设置新的光标位置

.is_line_feed:
.is_carriage_return:
    ; 处理换行和回车符
    xor dx, dx
    mov ax, bx
    mov si, 80
    div si
    sub bx, dx

.is_carriage_return_end:
    add bx, 80
    cmp bx, 2000
.is_line_feed_end:
    jl .set_cursor

.roll_screen:
    ; 滚屏处理
    cld
    mov ecx, 960
    mov esi, 0xb80a0
    mov edi, 0xb8000
    rep movsd

    ; 清空最后一行
    mov ebx, 3840
    mov ecx, 80
.cls:
    mov word [gs:ebx], 0x0720
    add ebx, 2
    loop .cls
    mov bx, 1920

.set_cursor:
    ; 设置光标位置
    mov dx, 0x03d4
    mov al, 0x0e
    out dx, al
    mov dx, 0x03d5
    mov al, bh
    out dx, al

    mov dx, 0x03d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x03d5
    mov al, bl
    out dx, al

.put_char_done:
    popad
    ret

;--------------------------------------------
; put_str 通过put_char来打印以0字符结尾的字符串
; C声明原型：void put_str(const char *str);
;--------------------------------------------
; 输入：栈中参数为打印的字符串
; 输出：无
put_str:
    push ebx                   ; 备份寄存器ebx
    push ecx                   ; 备份寄存器ecx
    xor ecx, ecx               ; 清空ecx用于存储参数
    mov ebx, [esp + 12]        ; 从栈中得到待打印的字符串地址
.goon:
    mov cl, [ebx]              ; 获取字符串中的字符
    cmp cl, 0                  ; 检查是否到字符串结尾
    jz .str_over               ; 如果是字符串结尾，跳转到结束部分
    push ecx                   ; 为put_char函数传递参数
    call put_char
    add esp, 4                 ; 回收参数所占的栈空间
    inc ebx                    ; 使ebx指向下一个字符
    jmp .goon
.str_over:
    pop ecx                    ; 恢复寄存器ecx
    pop ebx                    ; 恢复寄存器ebx
    ret

;--------------------   将小端字节序的数字变成对应的ascii后，倒置   -----------------------
; 功能描述: 将栈中参数中的整数以16进制形式输出，不打印前缀0x
; 输入：栈中参数为待打印的数字
; 输出：在屏幕上打印16进制数字，如输入10进制15，输出字符'f'
; C声明原型：void put_int(int n);
;-----------------------------------------------------------------------------------
put_int:
    pushad                      ; 备份所有寄存器
    mov ebp, esp                ; 设置基指针寄存器
    mov eax, [ebp + 4*9]        ; 获取待打印的数字，位于栈中call的返回地址占4字节+pushad的8个4字节

    mov edx, eax                ; 复制数字到edx，用于转换过程
    mov edi, 7                  ; 指定初始偏移量，从缓冲区的末尾开始
    mov ecx, 8                  ; 16进制数字的位数为8
    mov ebx, put_int_buffer     ; 设置缓冲区指针

.16based_4bits:
    ; 处理每一个16进制数字
    and edx, 0x0000000F         ; 取数字的低4位
    cmp edx, 9
    jg .is_A2F
    add edx, '0'                ; 数字0～9转换为字符
    jmp .store

.is_A2F:
    sub edx, 10
    add edx, 'A'                ; 数字A～F转换为字符

.store:
    mov [ebx + edi], dl         ; 存储字符到缓冲区
    dec edi                     ; 向缓冲区前进一位
    shr eax, 4                  ; 处理下一个16进制数字
    mov edx, eax
    loop .16based_4bits

.ready_to_print:
    ; 去掉高位连续的0
    inc edi
.skip_prefix_0:
    cmp edi, 8
    je .full0
.go_on_skip:
    mov cl, [put_int_buffer + edi]
    inc edi
    cmp cl, '0'
    je .skip_prefix_0
    dec edi
    jmp .put_each_num

.full0:
    mov cl, '0'                 ; 如果全为0，则打印单个0
.put_each_num:
    ; 打印每一个非0字符
    push ecx
    call put_char
    add esp, 4
    inc edi
    mov cl, [put_int_buffer + edi]
    cmp edi, 8
    jl .put_each_num

    popad                       ; 恢复所有寄存器
    ret
