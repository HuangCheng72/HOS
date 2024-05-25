//
// Created by huangcheng on 2024/5/25.
//

#include "kernel_idt.h"

void init_idt() {

    // 初始化 IDT，将所有中断描述符清零
    InterruptDescriptor *idt = (InterruptDescriptor *)IDT_BASE_ADDR;

    for (int i = 0; i < IDT_SIZE; ++i) {
        // 全部清空信息
        *((uint64_t *)(idt + i)) = 0;
    }

    // 以下是主片初始化数据，都带_m后缀
    PIC_ICW1 icw1_m = {
        .init = 1,                // 开始初始化为1
        .single = 0,              // 级联模式
        .interval4 = 0,           // 中断向量号的调用地址间隔为8字节
        .level_triggered = 0,     // 边沿触发模式
        .needs_icw4 = 1,          // 需要ICW4
        .reserved = 0             // 保留位，固定为0
    }; // 十六进制表示: 0x11

    PIC_ICW2 icw2_m = {
        .offset = 0x20            // 中断向量偏移量为0x20
    }; // 十六进制表示: 0x20

    PIC_ICW3_Master icw3_m = {
        .slave_on_ir0 = 0,        // IR0未连接从PIC
        .slave_on_ir1 = 0,        // IR1未连接从PIC
        .slave_on_ir2 = 1,        // IR2连接从PIC
        .slave_on_ir3 = 0,        // IR3未连接从PIC
        .slave_on_ir4 = 0,        // IR4未连接从PIC
        .slave_on_ir5 = 0,        // IR5未连接从PIC
        .slave_on_ir6 = 0,        // IR6未连接从PIC
        .slave_on_ir7 = 0         // IR7未连接从PIC
    }; // 十六进制表示: 0x04

    PIC_ICW4 icw4_m = {
        .mode_8086 = 1,           // 8086/88模式
        .auto_eoi = 0,            // 非自动EOI
        .buffered = 0,            // 非缓冲模式
        .master_slave = 0,        // 主PIC
        .special_fully_nested = 0,// 不支持特殊全嵌套
        .reserved = 0             // 保留位，固定为0
    }; // 十六进制表示: 0x01


    // 初始化PIC主片
    // 将结构体强制转换为 uint8_t 类型并写入端口
    // 因为任何C编译器都不允许结构体强转基本数据类型，所以需要迂回路线
    // 也就是取址，然后转指针类型，再解引用
    // 或者建立一个联合体（类似于set_interrupt_descriptor中的方法）

    outb (PIC_M_CTRL, *((uint8_t *)&icw1_m));   // ICW1: 初始化命令
    outb (PIC_M_DATA, *((uint8_t *)&icw2_m));   // ICW2: 主片中断向量偏移量0x20（该PIC最高优先级的IRQ 0中断，中断描述符在IDT[0x20]处）
    outb (PIC_M_DATA, *((uint8_t *)&icw3_m));   // ICW3: 主片IR2连接从片
    outb (PIC_M_DATA, *((uint8_t *)&icw4_m));   // ICW4: 8086模式, 正常EOI

    // 以下是从片初始化数据，都带_s后缀
    PIC_ICW1 icw1_s = {
        .init = 1,                // 开始初始化为1
        .single = 0,              // 级联模式
        .interval4 = 0,           // 中断向量号的调用地址间隔为8字节
        .level_triggered = 0,     // 边沿触发模式
        .needs_icw4 = 1,          // 需要ICW4
        .reserved = 0             // 保留位，固定为0
    }; // 十六进制表示: 0x11

    PIC_ICW2 icw2_s = {
        .offset = 0x28            // 中断向量偏移量为0x28
    }; // 十六进制表示: 0x28

    PIC_ICW3_Slave icw3_s = {
        .cascade_identity = 2,    // 连接到主PIC的IR2线
        .reserved = 0             // 保留位，固定为0
    }; // 十六进制表示: 0x02

    PIC_ICW4 icw4_s = {
        .mode_8086 = 1,           // 8086/88模式
        .auto_eoi = 0,            // 非自动EOI
        .buffered = 0,            // 非缓冲模式
        .master_slave = 0,        // 主PIC
        .special_fully_nested = 0,// 不支持特殊全嵌套
        .reserved = 0             // 保留位，固定为0
    }; // 十六进制表示: 0x01

    outb (PIC_S_CTRL, *((uint8_t *)&icw1_s));   // ICW1: 初始化命令
    outb (PIC_S_DATA, *((uint8_t *)&icw2_s));   // ICW2: 从片中断向量偏移量0x28（该PIC最高优先级的IRQ 8中断，中断描述符在IDT[0x28]处）
    outb (PIC_S_DATA, *((uint8_t *)&icw3_s));   // ICW3: 从片连接主片IR2
    outb (PIC_S_DATA, *((uint8_t *)&icw4_s));   // ICW4: 8086模式, 正常EOI

    // 初始化完成才能设置允许什么中断，屏蔽什么中断
    // 对IMR发送屏蔽哪些中断放行哪些中断是直接对数据端口写你的要求

    // 初始化主片OCW1数据
    PIC_Master_OCW1 ocw1_m = {
        .irq0 = 1,      // 屏蔽 IRQ0
        .irq1 = 1,      // 屏蔽 IRQ1
        .irq2 = 1,      // 屏蔽 IRQ2
        .irq3 = 1,      // 屏蔽 IRQ3
        .irq4 = 1,      // 屏蔽 IRQ4
        .irq5 = 1,      // 屏蔽 IRQ5
        .irq6 = 1,      // 屏蔽 IRQ6
        .irq7 = 1,      // 屏蔽 IRQ7
    }; // 十六进制表示: 0xff

    // 初始化从片OCW1数据
    PIC_Slave_OCW1 ocw1_s = {
        .irq8 = 1,       // 屏蔽 IRQ8
        .irq9 = 1,       // 屏蔽 IRQ9
        .irq10 = 1,      // 屏蔽 IRQ10
        .irq11 = 1,      // 屏蔽 IRQ11
        .irq12 = 1,      // 屏蔽 IRQ12
        .irq13 = 1,      // 屏蔽 IRQ13
        .irq14 = 1,      // 屏蔽 IRQ14
        .irq15 = 1,      // 屏蔽 IRQ15
    }; // 十六进制表示: 0xff

    // 对数据端口写入OCW1数据，PIC就知道你要屏蔽哪个中断，允许哪个中断了
    outb (PIC_M_DATA, *((uint8_t*)(&ocw1_m)));   // 屏蔽主PIC所有中断
    outb (PIC_S_DATA, *((uint8_t*)(&ocw1_s)));   // 屏蔽从PIC所有中断

    // 256 个中断描述符，8 * 256 - 1 = 2047，即0x7ff
    // 虚拟地址加载IDT
    load_idt( 256 * 8 - 1 ,IDT_BASE_ADDR + HIGH_ADDR_OFFSET);

}


void set_interrupt_descriptor(uint32_t index, void (*interrupt_handler)()) {

    InterruptDescriptor *idt = (InterruptDescriptor *)IDT_BASE_ADDR;

    // 一种用结构体和联合体快速分离高位和低位信息的方法（对分离其他位信息同样适用）
    typedef struct {
        uint16_t offset_low;
        uint16_t offset_high;
    } temp_interrupt_handler_struct;
    typedef union {
        void *ptr;              // 32位平台上所有指针本质上都是uin32_t，64位平台上所有指针都是uint64_t
        temp_interrupt_handler_struct value;
    } temp_interrupt_handler_union;

    temp_interrupt_handler_union tempUnion;
    tempUnion.ptr = interrupt_handler;

    (idt + index)->offset_low = tempUnion.value.offset_low;
    (idt + index)->offset_high = tempUnion.value.offset_high;

    (idt + index)->rpl = 0;     // 内核当然要最高特权
    (idt + index)->ti = 0;      // 毫无疑问GDT
    (idt + index)->index = 1;   // 内核代码段在GDT里面的index是1

    (idt + index)->ist = 0;     // 通常设置为0
    (idt + index)->zero = 0;    // 保留位，设置为0

    (idt + index)->type = 0xE;  // 固定为中断门类型
    (idt + index)->s = 0;       // 描述符类型，0=系统
    (idt + index)->dpl = 0;     // 固定为0
    (idt + index)->p = 1;       // 固定为存在

}
