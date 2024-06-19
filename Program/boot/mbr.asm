%include "boot.inc"
; 加载配置，这样以后调整都方便
SECTION MBR vstart=0x7c00
; 入口地址0x7c00是MBR的惯例，BIOS只要发现MBR，一律加载到这个地址
; 在开始就设置每个寄存器的初始值
    mov ax,cs
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov fs,ax
; cs一开始是0，设置ds、es、ss、fs四个都是0
    mov sp,0x7c00
; sp存储的是栈顶指针，在MBR一开始必须是0x7c00（这本身也是MBR的加载位置，栈是向低地址处生长的，所以不会覆盖MBR的数据）
    mov ax,0xb800
; 0xb800，VGA时代对应显存的起始位置，放到gs寄存器里面
    mov gs,ax

; 清屏
    mov     ax, 0600h
    mov     bx, 0700h
    mov     cx, 0
    mov     dx, 184fh
    int     10h

; 读入Loader和内核

    ; 设置磁盘地址包
    mov byte [disk_address_packet_loader], 16              ; 包大小
    mov byte [disk_address_packet_loader + 1], 0           ; 保留字段
    mov word [disk_address_packet_loader + 2], 4         ; 要读取的扇区数
    mov word [disk_address_packet_loader + 4], 0           ; 缓冲区偏移量（数值是整数，自然可以写死为0）
    ; 计算目标地址段
    mov ax, LOADER_BASE_ADDR
    shr ax, 4
    mov word [disk_address_packet_loader + 6], ax          ; 缓冲区段
    xor ax, ax
    mov word [disk_address_packet_loader + 8], LOADER_START_SECTOR      ; 起始LBA地址（低32位）主要是因为LOADER_START_SECTOR小，所以可以直接写到低位，不然就要拆分了
    mov word [disk_address_packet_loader + 12], 0                       ; 起始LBA地址（高32位）

    ; 设置BIOS中断0x13的参数
    mov ah, 0x42                       ; 扩展读取功能
    mov dl, 0x80                       ; 驱动器号，第一个硬盘
    lea si, [disk_address_packet_loader]      ; 磁盘地址包指针
    int 0x13                           ; 调用BIOS中断
    jc error                           ; 如果出错，跳转到错误处理

    jmp LOADER_BASE_ADDR               ; 进入Loader进行后续设置

error:
    jmp $                       ; 错误处理，死循环



; 地址包，所谓DAP，这是BIOS 0x13号中断的0x42功能所需的参数列表
disk_address_packet_loader:
    db 16                          ; 包大小
    db 0                           ; 保留字段
    dw 0                           ; 要读取的扇区数
    dw 0                           ; 缓冲区偏移量（这两个值和0x02功能里面的bx、es同义）
    dw 0                           ; 缓冲区段
    dd 0                           ; 起始LBA地址（低32位）
    dd 0                           ; 起始LBA地址（高32位）

times 510-($-$$) db 0
; 这行代码的作用是填充当前代码段，使其总长度达到510字节。
; MBR的总大小是512字节，其中最后两个字节是固定的引导标识符 0x55 和 0xAA，所以需要填充前面的字节。
; $ 表示当前地址，$$ 表示当前段的起始地址，($-$$) 计算出当前代码段的大小，然后 510-($-$$) 计算出需要填充的字节数。
; 用 db 0 填充这些字节。

db 0x55,0xaa		; 这两个字节是引导记录的标识符，表示这是一个有效的MBR。
