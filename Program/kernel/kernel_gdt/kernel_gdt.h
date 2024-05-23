//
// Created by huangcheng on 2024/5/23.
//

#ifndef HOS_KERNEL_GDT_H
#define HOS_KERNEL_GDT_H

// --------------   gdt描述符属性  -------------
#define DESC_G_4K   0x80000000
#define DESC_D_32   0x40000000
#define DESC_L      0x20000000
#define DESC_AVL    0x10000000
#define DESC_LIMIT_CODE2  0x0F000000
#define DESC_LIMIT_DATA2  DESC_LIMIT_CODE2
#define DESC_LIMIT_VIDEO2 0x00000000
#define DESC_P      0x00008000
#define DESC_DPL_0  0x00000000
#define DESC_DPL_1  0x00002000
#define DESC_DPL_2  0x00004000
#define DESC_DPL_3  0x00006000
#define DESC_S_CODE 0x00001000
#define DESC_S_DATA DESC_S_CODE
#define DESC_S_SYS  0x00000000
#define DESC_TYPE_CODE 0x00000800
#define DESC_TYPE_DATA 0x00000200

#define DESC_CODE_HIGH4 (DESC_G_4K | DESC_D_32 | DESC_L | DESC_AVL | DESC_LIMIT_CODE2 | DESC_P | DESC_DPL_0 | DESC_S_CODE | DESC_TYPE_CODE)
#define DESC_DATA_HIGH4 (DESC_G_4K | DESC_D_32 | DESC_L | DESC_AVL | DESC_LIMIT_DATA2 | DESC_P | DESC_DPL_0 | DESC_S_DATA | DESC_TYPE_DATA)
#define DESC_VIDEO_HIGH4 (DESC_G_4K | DESC_D_32 | DESC_L | DESC_AVL | DESC_LIMIT_VIDEO2 | DESC_P | DESC_DPL_0 | DESC_S_DATA | DESC_TYPE_DATA | 0x0b)

// --------------   段选择子属性  ---------------
#define RPL0  0x0
#define RPL1  0x1
#define RPL2  0x2
#define RPL3  0x3
#define TI_GDT  0x0
#define TI_LDT  0x4

#include "../../lib/lib.h"

// 段选择子结构体形式
typedef struct {
    uint16_t limit_low;  // 段界限 0~15
    uint16_t base_low;   // 段基址 0~15
    uint8_t base_mid;    // 段基址 16~23
    uint8_t access;      // 段存在位、特权级、描述符类型
    uint8_t granularity; // 其他标志、段界限 16~19
    uint8_t base_high;   // 段基址 24~31
} SegmentDescriptor;

// GDT指针结构体形式
typedef struct {
    uint16_t limit;      // 段界限
    uint32_t base;       // 段基址
} GdtPtr;

// 1MB 换算成十六进制是 0x100000，低于这个的我们就预留给内核、MBR、Loader使用
// 在之前的设计里面0到0x900给了栈，0x900到0x1500给了MBR，0x1500往后留给内核


#define GDT_BASE_ADDR 0x903             // gdt在内存中的起始地址
#define GDT_PTR_ADDR 0xa03              // gdt_ptr在内存中的地址
#define HIGH_ADDR_OFFSET 0xc0000000     // 往高地址的偏移量

// 调整GDT到高地址处（内核所在的虚拟地址上）
void setup_gdt();

#endif //HOS_KERNEL_GDT_H
