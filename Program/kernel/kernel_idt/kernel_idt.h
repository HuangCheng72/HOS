//
// Created by huangcheng on 2024/5/25.
//

#ifndef HOS_KERNEL_IDT_H
#define HOS_KERNEL_IDT_H

#include "../../lib/lib.h"

// 中断描述符结构体形式
typedef struct {
    uint64_t offset_low : 16;   // 位0到位15: 偏移量低16位，中断处理程序的地址（在段中的偏移地址）的低16位。

    // --------------- 这部分是之前在GDT里面提到的段选择子selector ---------------

    uint16_t rpl : 2;           // 位16到位17: 段选择子的位0-1: 请求特权级 (Requested Privilege Level, RPL，0最高特权级，3最低特权级)
    uint16_t ti : 1;            // 位18: 段选择子的位2: 表指示符 (Table Indicator, 0=GDT, 1=LDT)
    uint16_t index : 13;        // 位19到位31: 段选择子的位3-15: 段索引 (Segment Index)

    // --------------- 这部分是之前在GDT里面提到的段选择子selector ---------------

    // --------------- 这部分是保留信息dcount，一般用不到设置，全都是0 ---------------

    uint64_t ist : 3;           // 位32到位34: 中断堆栈表 (Interrupt Stack Table) 索引，通常为0。
    uint64_t zero : 5;          // 位35到位39: 置零，保留位，设置为0。

    // --------------- 这部分是保留信息dcount，一般用不到设置，全都是0 ---------------

    // --------------- 这部分是access_right，访问权限，《操作系统真象还原》里面叫attribute（属性） ---------------

    uint64_t type : 4;          // 位40到位43: 描述符类型（中断门、陷阱门等）。0xE（1110）: 32位中断门（Interrupt Gate），0xF（1111）: 32位陷阱门（Trap Gate）。
    uint64_t s : 1;             // 位44: 描述符权限，0=系统, 1=代码或数据。对于中断和陷阱门，此位通常为0。
    uint64_t dpl : 2;           // 位45到位46: 描述符特权级 (Descriptor Privilege Level)，0为最高特权级，3为最低特权级。
    uint64_t p : 1;             // 位47: 段存在位 (Present)，1表示描述符有效。

    // --------------- 这部分是access_right，访问权限，《操作系统真象还原》里面叫attribute（属性） ---------------

    uint64_t offset_high : 16;  // 位48到位63: 偏移量高16位，中断处理程序的地址（在段中的偏移地址）的高16位。
} InterruptDescriptor;

// IDT指针结构体，和GDT指针完全一样
// 如果要使用，必须禁止自动内存对齐该结构体
typedef struct {
    uint16_t limit;      // IDT界限
    uint32_t base;       // IDT基址
} IdtPtr;

// 1MB 换算成十六进制是 0x100000，低于这个的我们就预留给内核、MBR、Loader使用
// 在之前的设计里面0到0x900给了栈，0x900到0x1500给了MBR，0x1500往后留给内核

// 一个IDT描述符的大小是8字节
// 一般来说中断挺多的，要预留多一些，256个就很多了
// 不过我们也不考虑那么多，预留128个位置已经很充足了
// 那么段界限就是 8 * 256 - 1 = 2047，十六进制下，2048是0x800
// 0x6000 + 0x800 = 0x6800

#define IDT_BASE_ADDR 0x6000            // idt在内存中的起始地址
#define IDT_SIZE 256                    // 预留的IDT描述符个数
#define HIGH_ADDR_OFFSET 0xc0000000     // 往高地址的偏移量

// 段选择子的计算过程
#define	 RPL0  0
#define	 RPL1  1
#define	 RPL2  2
#define	 RPL3  3

#define TI_GDT 0
#define TI_LDT 1

#define SELECTOR_K_CODE	   ((1 << 3) + (TI_GDT << 2) + RPL0)    // 系统代码段
#define SELECTOR_K_DATA	   ((2 << 3) + (TI_GDT << 2) + RPL0)    // 系统数据段
#define SELECTOR_K_STACK   SELECTOR_K_DATA                      // 系统堆栈段
#define SELECTOR_K_GS	   ((3 << 3) + (TI_GDT << 2) + RPL0)    // 系统视频段

// 大部分普通x86平台上的可编程中断控制器PIC基本上都是8259A这个型号
// 它分主从两片PIC（master-slave），从片PIC连接到主片的一个管脚，从PIC发出的中断，通过主PIC的IRQ2中断提醒CPU
// 实际上CPU不知道从PIC发出了中断，只是IRQ2中断的发生的时候，它才去读从PIC到底发生了什么
// PIC自己其实也有寄存器，存放待处理的数据，我们通过其数据端口去读，通过其端口去写。
#define PIC_M_CTRL 0x20	       // 主PIC的控制端口是0x20
#define PIC_M_DATA 0x21	       // 主PIC的数据端口是0x21
#define PIC_S_CTRL 0xa0	       // 从PIC的控制端口是0xa0
#define PIC_S_DATA 0xa1	       // 从PIC的数据端口是0xa1

// 给PIC的IMR写的数据，转换成结构体位域的形式
// 主片IMR的结构体
typedef struct {
    uint8_t irq0 : 1 ;          // 是否屏蔽 IRQ 0 中断，为1则屏蔽，为0则不屏蔽
    uint8_t irq1 : 1 ;          // 是否屏蔽 IRQ 1 中断，为1则屏蔽，为0则不屏蔽（如果屏蔽整个从片，这个也必须屏蔽）
    uint8_t irq2 : 1 ;          // 是否屏蔽 IRQ 2 中断，为1则屏蔽，为0则不屏蔽
    uint8_t irq3 : 1 ;          // 是否屏蔽 IRQ 3 中断，为1则屏蔽，为0则不屏蔽
    uint8_t irq4 : 1 ;          // 是否屏蔽 IRQ 4 中断，为1则屏蔽，为0则不屏蔽
    uint8_t irq5 : 1 ;          // 是否屏蔽 IRQ 5 中断，为1则屏蔽，为0则不屏蔽
    uint8_t irq6 : 1 ;          // 是否屏蔽 IRQ 6 中断，为1则屏蔽，为0则不屏蔽
    uint8_t irq7 : 1 ;          // 是否屏蔽 IRQ 7 中断，为1则屏蔽，为0则不屏蔽
} PIC_Master_IMR;

// 从片IMR的结构体
typedef struct {
    uint8_t irq8 : 1 ;          // 是否屏蔽 IRQ 8 中断，为1则屏蔽，为0则不屏蔽
    uint8_t irq9 : 1 ;          // 是否屏蔽 IRQ 9 中断，为1则屏蔽，为0则不屏蔽
    uint8_t irq10 : 1 ;         // 是否屏蔽 IRQ 10 中断，为1则屏蔽，为0则不屏蔽
    uint8_t irq11 : 1 ;         // 是否屏蔽 IRQ 11 中断，为1则屏蔽，为0则不屏蔽
    uint8_t irq12 : 1 ;         // 是否屏蔽 IRQ 12 中断，为1则屏蔽，为0则不屏蔽
    uint8_t irq13 : 1 ;         // 是否屏蔽 IRQ 13 中断，为1则屏蔽，为0则不屏蔽
    uint8_t irq14 : 1 ;         // 是否屏蔽 IRQ 14 中断，为1则屏蔽，为0则不屏蔽
    uint8_t irq15 : 1 ;         // 是否屏蔽 IRQ 15 中断，为1则屏蔽，为0则不屏蔽
} PIC_Slave_IMR;

// IDT初始化
void init_idt();

/**
 * 设置中断描述符
 * @param index                 在IDT中的索引
 * @param interrupt_handler     指向中断处理程序的函数指针
 */
void set_interrupt_descriptor(uint32_t index, void (*interrupt_handler)());


#endif //HOS_KERNEL_IDT_H
