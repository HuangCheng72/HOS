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

; 读入Loader
   mov eax,LOADER_START_SECTOR	 ; 起始扇区lba地址
   mov bx,LOADER_BASE_ADDR       ; 写入的地址
   mov cx,4			 ; 待读入的扇区数
   call rd_disk_m_16		 ; 以下读取程序的起始部分（一个扇区）
   

   jmp LOADER_BASE_ADDR

;-------------------------------------------------------------------------------
;功能:读取硬盘n个扇区，16位
rd_disk_m_16:
;-------------------------------------------------------------------------------
    ; 参数说明：
    ; eax = LBA扇区号
    ; ebx = 将数据写入的内存地址
    ; ecx = 读入的扇区数

    mov esi, eax          ; 备份eax，保存LBA扇区号
    mov di, cx            ; 备份cx，保存要读取的扇区数

    ; 第1步：设置要读取的扇区数
    mov dx, SECTOR_COUNT_REG ; 选择扇区计数寄存器端口
    mov al, cl            ; 将要读取的扇区数放入al
    out dx, al            ; 输出到扇区计数寄存器

    mov eax, esi          ; 恢复eax，包含LBA扇区号

    ; 第2步：将LBA地址存入0x1f3 ~ 0x1f6

    ; LBA地址7~0位写入端口SECTOR_NUM_REG
    mov dx, SECTOR_NUM_REG
    out dx, al

    ; LBA地址15~8位写入端口CYL_LOW_REG
    mov cl, 8
    shr eax, cl
    mov dx, CYL_LOW_REG
    out dx, al

    ; LBA地址23~16位写入端口CYL_HIGH_REG
    shr eax, cl
    mov dx, CYL_HIGH_REG
    out dx, al

    ; LBA地址27~24位写入端口DRIVE_HEAD_REG
    shr eax, cl
    and al, 0x0f       ; 只保留LBA地址的低4位
    or al, 0xe0        ; 设置7~4位为1110，表示LBA模式
    mov dx, DRIVE_HEAD_REG
    out dx, al

    ; 第3步：向STATUS_CMD_REG端口写入读命令，0x20
    mov dx, STATUS_CMD_REG
    mov al, 0x20
    out dx, al

    ; 第4步：检测硬盘状态
    .not_ready:
        nop
        in al, dx
        and al, 0x88       ; 检查硬盘控制器状态：第4位为1表示准备好数据传输，第7位为1表示硬盘忙
        cmp al, 0x08
        jnz .not_ready     ; 若未准备好，继续等待

    ; 第5步：从DATA_REG端口读数据
    mov ax, di
    mov dx, 256
    mul dx
    mov cx, ax         ; di为要读取的扇区数，一个扇区有512字节，每次读入一个字，共需di*512/2次
    mov dx, DATA_REG
    .go_on_read:
        in ax, dx
        mov [bx], ax
        add bx, 2
        loop .go_on_read
        ret

times 510-($-$$) db 0
; 这行代码的作用是填充当前代码段，使其总长度达到510字节。
; MBR的总大小是512字节，其中最后两个字节是固定的引导标识符 0x55 和 0xAA，所以需要填充前面的字节。
; $ 表示当前地址，$$ 表示当前段的起始地址，($-$$) 计算出当前代码段的大小，然后 510-($-$$) 计算出需要填充的字节数。
; 用 db 0 填充这些字节。

db 0x55,0xaa		; 这两个字节是引导记录的标识符，表示这是一个有效的MBR。
