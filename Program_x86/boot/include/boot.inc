; 配置加载Loader和加载内核需要用到的一些宏定义
;-------------	 loader和kernel   ----------
LOADER_BASE_ADDR    equ 0x900   ; 定义加载器的基址
LOADER_START_SECTOR equ 0x2     ; 定义加载器的起始扇区

KERNEL_BASE_ADDR    equ 0xe000  ; 定义内核的基址
KERNEL_START_SECTOR equ 0x8     ; 定义内核的起始扇区

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

;--------------   gdt描述符属性  -------------
; DESC_G_4K: 4KB 粒度标志
DESC_G_4K   equ	  1_00000000000000000000000b   
; DESC_D_32: 默认操作数大小（32位）
DESC_D_32   equ	   1_0000000000000000000000b
; DESC_L: 64位代码段标志（此处设置为0）
DESC_L	    equ	    0_000000000000000000000b
; DESC_AVL: 系统软件可用位（通常设置为0）
DESC_AVL    equ	     0_00000000000000000000b
; DESC_LIMIT_CODE2: 代码段的段限制高4位
DESC_LIMIT_CODE2  equ 1111_0000000000000000b
; DESC_LIMIT_DATA2: 数据段的段限制高4位，与代码段相同
DESC_LIMIT_DATA2  equ DESC_LIMIT_CODE2
; DESC_LIMIT_VIDEO2: 视频段的段限制高4位
DESC_LIMIT_VIDEO2  equ 0000_000000000000000b
; DESC_P: 段存在标志
DESC_P	    equ		  1_000000000000000b
; DESC_DPL_0: 描述符特权级0
DESC_DPL_0  equ		   00_0000000000000b
; DESC_DPL_1: 描述符特权级1
DESC_DPL_1  equ		   01_0000000000000b
; DESC_DPL_2: 描述符特权级2
DESC_DPL_2  equ		   10_0000000000000b
; DESC_DPL_3: 描述符特权级3
DESC_DPL_3  equ		   11_0000000000000b
; DESC_S_CODE: 代码段
DESC_S_CODE equ		     1_000000000000b
; DESC_S_DATA: 数据段，与代码段相同
DESC_S_DATA equ	  DESC_S_CODE
; DESC_S_sys: 系统段
DESC_S_sys  equ		     0_000000000000b
; DESC_TYPE_CODE: 代码段类型属性（可执行、不可读取、已访问位清0）
DESC_TYPE_CODE  equ	      1000_00000000b	;x=1,c=0,r=0,a=0  
; DESC_TYPE_DATA: 数据段类型属性（不可执行、向上扩展、可写、已访问位清0）
DESC_TYPE_DATA  equ	      0010_00000000b	;x=0,e=0,w=1,a=0 

; DESC_CODE_HIGH4: 代码段描述符的高32位（包括基地址、段限制、标志等）
DESC_CODE_HIGH4 equ (0x00 << 24) + DESC_G_4K + DESC_D_32 + DESC_L + DESC_AVL + DESC_LIMIT_CODE2 + DESC_P + DESC_DPL_0 + DESC_S_CODE + DESC_TYPE_CODE + 0x00
; DESC_DATA_HIGH4: 数据段描述符的高32位（包括基地址、段限制、标志等）
DESC_DATA_HIGH4 equ (0x00 << 24) + DESC_G_4K + DESC_D_32 + DESC_L + DESC_AVL + DESC_LIMIT_DATA2 + DESC_P + DESC_DPL_0 + DESC_S_DATA + DESC_TYPE_DATA + 0x00
; DESC_VIDEO_HIGH4: 视频段描述符的高32位（包括基地址、段限制、标志等）
DESC_VIDEO_HIGH4 equ (0x00 << 24) + DESC_G_4K + DESC_D_32 + DESC_L + DESC_AVL + DESC_LIMIT_VIDEO2 + DESC_P + DESC_DPL_0 + DESC_S_DATA + DESC_TYPE_DATA + 0x0b

;--------------   段选择子属性  ---------------
; RPL0: 请求特权级0
RPL0  equ   00b
; RPL1: 请求特权级1
RPL1  equ   01b
; RPL2: 请求特权级2
RPL2  equ   10b
; RPL3: 请求特权级3
RPL3  equ   11b
; TI_GDT: 表示选择子指向GDT（全局描述符表）
TI_GDT	 equ   000b
; TI_LDT: 表示选择子指向LDT（局部描述符表）
TI_LDT	 equ   100b
