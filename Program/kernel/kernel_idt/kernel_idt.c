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


    // 这里发送的关键字大部分情况下都是固定的
    // 初始化PIC主片
    outb (PIC_M_CTRL, 0x11);   // ICW1: 初始化命令
    outb (PIC_M_DATA, 0x20);   // ICW2: 主片中断向量偏移地址0x20（该PIC最高优先级的IRQ 0中断，中断描述符在IDT[0x20]处）
    outb (PIC_M_DATA, 0x04);   // ICW3: 主片IR2连接从片
    outb (PIC_M_DATA, 0x01);   // ICW4: 8086模式, 正常EOI

    // 初始化PIC从片
    outb (PIC_S_CTRL, 0x11);   // ICW1: 初始化命令
    outb (PIC_S_DATA, 0x28);   // ICW2: 从片中断向量偏移地址0x28（该PIC最高优先级的IRQ 8中断，中断描述符在IDT[0x28]处）
    outb (PIC_S_DATA, 0x02);   // ICW3: 从片连接主片IR2
    outb (PIC_S_DATA, 0x01);   // ICW4: 8086模式, 正常EOI

    // 初始化完成才能设置允许什么中断，屏蔽什么中断
    // 对IMR发送屏蔽哪些中断放行哪些中断是直接对数据端口写你的要求

    PIC_Master_IMR master_imr;
    master_imr.irq0 = 1;  // 屏蔽 IRQ0
    master_imr.irq1 = 1;  // 屏蔽 IRQ1
    master_imr.irq2 = 1;  // 屏蔽 IRQ2
    master_imr.irq3 = 1;  // 屏蔽 IRQ3
    master_imr.irq4 = 1;  // 屏蔽 IRQ4
    master_imr.irq5 = 1;  // 屏蔽 IRQ5
    master_imr.irq6 = 1;  // 屏蔽 IRQ6
    master_imr.irq7 = 1;  // 屏蔽 IRQ7

    // 初始化从片IMR
    PIC_Slave_IMR slave_imr;
    slave_imr.irq8 = 1;  // 屏蔽 IRQ8
    slave_imr.irq9 = 1;  // 屏蔽 IRQ9
    slave_imr.irq10 = 1; // 屏蔽 IRQ10
    slave_imr.irq11 = 1; // 屏蔽 IRQ11
    slave_imr.irq12 = 1; // 屏蔽 IRQ12
    slave_imr.irq13 = 1; // 屏蔽 IRQ13
    slave_imr.irq14 = 1; // 屏蔽 IRQ14
    slave_imr.irq15 = 1; // 屏蔽 IRQ15


    // 将结构体强制转换为 uint8_t 类型并写入IMR寄存器
    // 因为任何C编译器都不允许结构体强转基本数据类型，所以需要迂回路线
    // 也就是取址，然后转指针类型，再解引用
    // 或者建立一个联合体
    // 联合体里面放一个PIC_Master_IMR和另一个uint8_t的成员value，这样写完了之后，value的值就是我们计算出来的值了

    outb (PIC_M_DATA, *((uint8_t*)(&master_imr)));  // 对数据端口写入IMR数据，PIC就知道你要屏蔽哪个中断，允许哪个中断了
    outb (PIC_S_DATA, *((uint8_t*)(&slave_imr)));

    // 256 个中断描述符，8 * 256 - 1 = 2047，即0x7ff
    // 虚拟地址加载IDT
    load_idt( 256 * 8 - 1 ,IDT_BASE_ADDR + HIGH_ADDR_OFFSET);

}

void set_interrupt_descriptor(uint32_t index, uint32_t handler_function, uint16_t selector) {
    InterruptDescriptor *idt = (InterruptDescriptor *)IDT_BASE_ADDR;

    (idt + index)->offset_low = handler_function & 0xFFFF;
    (idt + index)->offset_high = (handler_function >> 16) & 0xFFFF;

    (idt + index)->rpl = selector & 0x3;
    (idt + index)->ti = (selector >> 2) & 0x1;
    (idt + index)->index = (selector >> 3) & 0x1FFF;

    (idt + index)->ist = 0;     // 通常设置为0
    (idt + index)->zero = 0;    // 保留位，设置为0

    (idt + index)->type = 0xE;  // 固定为中断门类型
    (idt + index)->s = 0;       // 描述符类型，0=系统
    (idt + index)->dpl = 0;     // 固定为0
    (idt + index)->p = 1;       // 固定为存在

}
