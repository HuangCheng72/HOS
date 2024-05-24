//
// Created by huangcheng on 2024/5/23.
//

#ifndef HOS_KERNEL_GDT_H
#define HOS_KERNEL_GDT_H

// --------------   gdt描述符属性  -------------
#define DESC_TYPE_DATA 0x2          // 数据段类型
#define DESC_S_CODE_OR_DATA 1       // 描述符类型 (1=代码或数据)
#define DESC_DPL0 0x0               // 特权级 0
#define DESC_DPL1 0x1               // 特权级 1
#define DESC_DPL2 0x2               // 特权级 2
#define DESC_DPL3 0x3               // 特权级 3
#define DESC_PRESENT 1              // 段存在位
#define DESC_AVL 0                  // 可用位
#define DESC_LONG_MODE 0            // 不是64位代码段
#define DESC_DB 1                   // 默认操作大小 32位段
#define DESC_G_4K 1                 // 粒度 4KB

#include "../../lib/lib.h"

// 段选择子结构体形式
typedef struct {
    uint16_t rpl : 2;    // 位0-1: 请求特权级 (Requested Privilege Level, RPL，0最高特权级，3最低特权级)
    uint16_t ti : 1;     // 位2: 表指示符 (Table Indicator, 0=GDT, 1=LDT)
    uint16_t index : 13; // 位3-15: 段索引 (Segment Index)
} SegmentSelector;

// 段描述符结构体形式
typedef struct {
    uint64_t limit_low : 16;      // 位0-15: 段界限低16位
    uint64_t base_low : 16;       // 位16-31: 段基址低16位
    uint64_t base_mid : 8;        // 位32-39: 段基址中间8位
    uint64_t type : 4;            // 位40-43: 类型
    uint64_t s : 1;               // 位44: 描述符类型 (0=系统, 1=代码或数据)
    uint64_t dpl : 2;             // 位45-46: 特权级 (Descriptor Privilege Level)
    uint64_t p : 1;               // 位47: 段存在位
    uint64_t limit_high : 4;      // 位48-51: 段界限高4位
    uint64_t avl : 1;             // 位52: 可用位
    uint64_t l : 1;               // 位53: 64位代码段 (1=64位代码段)
    uint64_t db : 1;              // 位54: 默认操作大小 (0=16位段, 1=32位段)
    uint64_t g : 1;               // 位55: 粒度 (0=字节, 1=4KB)
    uint64_t base_high : 8;       // 位56-63: 段基址高8位
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
