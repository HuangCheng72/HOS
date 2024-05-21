; 配置加载Loader和加载内核需要用到的一些宏定义
;-------------	 loader和kernel   ----------
LOADER_BASE_ADDR    equ 0x900   ; 定义加载器的基址
LOADER_START_SECTOR equ 0x2     ; 定义加载器的起始扇区

;---------------    硬盘寄存器   -------------------
; Primary 通道
DATA_REG          equ 0x1f0  ; 数据寄存器
ERROR_REG         equ 0x1f1  ; 错误寄存器/预编译寄存器
SECTOR_COUNT_REG  equ 0x1f2  ; 扇区计数寄存器
SECTOR_NUM_REG    equ 0x1f3  ; 扇区号寄存器
CYL_LOW_REG       equ 0x1f4  ; 柱面号低字节寄存器
CYL_HIGH_REG      equ 0x1f5  ; 柱面号高字节寄存器
DRIVE_HEAD_REG    equ 0x1f6  ; 驱动器/磁头寄存器
STATUS_CMD_REG    equ 0x1f7  ; 状态寄存器/命令寄存器
CONTROL_REG       equ 0x3f6  ; 控制寄存器

; Secondary 通道
SEC_DATA_REG          equ 0x170  ; 数据寄存器
SEC_ERROR_REG         equ 0x171  ; 错误寄存器/预编译寄存器
SEC_SECTOR_COUNT_REG  equ 0x172  ; 扇区计数寄存器
SEC_SECTOR_NUM_REG    equ 0x173  ; 扇区号寄存器
SEC_CYL_LOW_REG       equ 0x174  ; 柱面号低字节寄存器
SEC_CYL_HIGH_REG      equ 0x175  ; 柱面号高字节寄存器
SEC_DRIVE_HEAD_REG    equ 0x176  ; 驱动器/磁头寄存器
SEC_STATUS_CMD_REG    equ 0x177  ; 状态寄存器/命令寄存器
SEC_CONTROL_REG       equ 0x376  ; 控制寄存器
