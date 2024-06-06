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

#include "../../lib/lib_kernel/lib_kernel.h"

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

/*
存储段描述符类型（代码段和数据段）
存储段描述符的 type 字段由 4 位组成，它们的具体含义如下：

代码段类型

8 (1000): 可执行 (Executable), 不可读 (Non-readable), 非一致性 (Non-conforming), 未访问 (Not accessed)
9 (1001): 可执行, 不可读, 非一致性, 已访问 (Accessed)
A (1010): 可执行, 可读 (Readable), 非一致性, 未访问
B (1011): 可执行, 可读, 非一致性, 已访问
C (1100): 可执行, 不可读, 一致性 (Conforming), 未访问
D (1101): 可执行, 不可读, 一致性, 已访问
E (1110): 可执行, 可读, 一致性, 未访问
F (1111): 可执行, 可读, 一致性, 已访问
数据段类型

0 (0000): 不可执行 (Non-executable), 不可写 (Non-writable), 向上扩展 (Expand-up), 未访问
1 (0001): 不可执行, 不可写, 向上扩展, 已访问
2 (0010): 不可执行, 可写 (Writable), 向上扩展, 未访问
3 (0011): 不可执行, 可写, 向上扩展, 已访问
4 (0100): 不可执行, 不可写, 向下扩展 (Expand-down), 未访问
5 (0101): 不可执行, 不可写, 向下扩展, 已访问
6 (0110): 不可执行, 可写, 向下扩展, 未访问
7 (0111): 不可执行, 可写, 向下扩展, 已访问
系统段描述符类型
系统段描述符的 type 字段同样由 4 位组成，它们的具体含义如下：

0 (0000): 保留
1 (0001): 16 位 TSS (可用状态)
2 (0010): LDT (局部描述符表)
3 (0011): 16 位 TSS (忙状态)
4 (0100): 16 位调用门 (Call Gate)
5 (0101): 任务门 (Task Gate)
6 (0110): 16 位中断门 (Interrupt Gate)
7 (0111): 16 位陷阱门 (Trap Gate)
8 (1000): 保留
9 (1001): 32 位 TSS (可用状态)
A (1010): 保留
B (1011): 32 位 TSS (忙状态)
C (1100): 32 位调用门 (Call Gate)
D (1101): 保留
E (1110): 32 位中断门 (Interrupt Gate)
F (1111): 32 位陷阱门 (Trap Gate)
 */

// GDT指针结构体形式
// 不能直接用，仅供参考，会有编译器自动内存对齐问题，要加上禁止内存自动对齐的__attribute__((packed))关键字
typedef struct {
    uint16_t limit;      // 段界限
    uint32_t base;       // 段基址
} __attribute__((packed)) GdtPtr;

// 1MB 换算成十六进制是 0x100000，低于这个的我们就预留给内核、MBR、Loader使用
// 在之前的设计里面0到0x900给了栈，0x900到0x1500给了MBR，0x1500往后留给内核

#define GDT_BASE_ADDR 0x903             // gdt在内存中的起始地址
#define GDT_PTR_ADDR 0xa03              // gdt_ptr在内存中的地址
#define HIGH_ADDR_OFFSET 0xc0000000     // 往高地址的偏移量

// GDT修改，挪到0x1500之后，和IDT放在一块吧（0x1500之前全部留给栈使用）
// IDT结束位置是0x10800
#define NEW_GDT_BASE_ADDR 0x10800
// GDT有32条，每条8字节，一共是256字节，即0x100
// 则GDT的结束地址是0x108ff（映射高地址就是0xc00108ff）
// TSS暂时先放在这里
#define TSS_ADDR 0xc0010900

// 调整GDT到高地址处（内核所在的虚拟地址上）
void setup_gdt();

// 定义 TSS 结构体
struct TSS {
    uint16_t previous_task_link;  // 前一个任务的 TSS 选择子，用于任务切换时保存前一个任务的 TSS
    uint16_t reserved0;           // 保留字段，未使用

    uint32_t esp0;                // 特权级 0 的栈指针，切换到内核态时使用
    uint16_t ss0;                 // 特权级 0 的栈段选择子
    uint16_t reserved1;           // 保留字段，未使用

    uint32_t esp1;                // 特权级 1 的栈指针，未使用
    uint16_t ss1;                 // 特权级 1 的栈段选择子
    uint16_t reserved2;           // 保留字段，未使用

    uint32_t esp2;                // 特权级 2 的栈指针，未使用
    uint16_t ss2;                 // 特权级 2 的栈段选择子
    uint16_t reserved3;           // 保留字段，未使用

    uint32_t cr3;                 // 页目录基地址寄存器，指向当前任务的页目录
    uint32_t eip;                 // 指令指针，保存任务执行时的指令地址
    uint32_t eflags;              // 标志寄存器，保存任务的状态标志
    uint32_t eax;                 // 通用寄存器，保存任务的 eax 寄存器值
    uint32_t ecx;                 // 通用寄存器，保存任务的 ecx 寄存器值
    uint32_t edx;                 // 通用寄存器，保存任务的 edx 寄存器值
    uint32_t ebx;                 // 通用寄存器，保存任务的 ebx 寄存器值
    uint32_t esp;                 // 栈指针，保存任务的 esp 寄存器值
    uint32_t ebp;                 // 基址指针，保存任务的 ebp 寄存器值
    uint32_t esi;                 // 源索引寄存器，保存任务的 esi 寄存器值
    uint32_t edi;                 // 目标索引寄存器，保存任务的 edi 寄存器值

    uint16_t es;                  // 段选择子，保存任务的 es 段选择子
    uint16_t reserved4;           // 保留字段，未使用
    uint16_t cs;                  // 段选择子，保存任务的 cs 段选择子
    uint16_t reserved5;           // 保留字段，未使用
    uint16_t ss;                  // 段选择子，保存任务的 ss 段选择子
    uint16_t reserved6;           // 保留字段，未使用
    uint16_t ds;                  // 段选择子，保存任务的 ds 段选择子
    uint16_t reserved7;           // 保留字段，未使用
    uint16_t fs;                  // 段选择子，保存任务的 fs 段选择子
    uint16_t reserved8;           // 保留字段，未使用
    uint16_t gs;                  // 段选择子，保存任务的 gs 段选择子
    uint16_t reserved9;           // 保留字段，未使用

    uint16_t ldt_segment_selector;  // 本地描述符表选择子，指向任务的 LDT
    uint16_t reserved10;            // 保留字段，未使用

    uint16_t debug_trap;            // 调试陷阱标志，如果设置则任务切换时产生调试陷阱
    uint16_t io_map_base_address;   // I/O 位图基址偏移，指向任务的 I/O 位图
} __attribute__((packed));          // 禁止编译器自动对齐，保证这个结构体紧凑

// 段选择子的计算过程
#define	 RPL0  0
#define	 RPL1  1
#define	 RPL2  2
#define	 RPL3  3

#define TI_GDT 0
#define TI_LDT 1

#define SELECTOR_K_CODE     ((1 << 3) + (TI_GDT << 2) + RPL0)   // 系统代码段
#define SELECTOR_K_DATA     ((2 << 3) + (TI_GDT << 2) + RPL0)   // 系统数据段
#define SELECTOR_K_STACK    SELECTOR_K_DATA                     // 系统堆栈段
#define SELECTOR_K_VIDEO    ((3 << 3) + (TI_GDT << 2) + RPL0)   // 系统视频段

#define SELECTOR_TSS        ((4 << 3) + (TI_GDT << 2) + RPL0)   // TSS段
#define SELECTOR_U_CODE     ((5 << 3) + (TI_GDT << 2) + RPL3)   // 用户代码段
#define SELECTOR_U_DATA     ((6 << 3) + (TI_GDT << 2) + RPL3)   // 用户数据段
#define SELECTOR_U_STACK    SELECTOR_U_DATA                     // 用户堆栈段

#endif //HOS_KERNEL_GDT_H
