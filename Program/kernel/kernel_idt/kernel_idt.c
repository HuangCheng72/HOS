//
// Created by huangcheng on 2024/5/25.
//

#include "kernel_idt.h"

#include "../interrupt_handler/interrupt_handler.h"


// 仅用于PIC的中断处理逻辑数组
void (*interrupt_handler_functions[256])(void) = { NULL };

// 统一中断分发器
void interrupt_dispatcher(uint32_t interrupt_number, uint32_t error_code){

    if(interrupt_number > 0x1f && interrupt_number < 0x30) {

        if (interrupt_handler_functions[interrupt_number]) {
            interrupt_handler_functions[interrupt_number]();
        }

        // 发送 EOI 信号
        PIC_OCW2 ocw2 = {
                .irq_level = 0,
                .reserved = 0,
                .eoi = 1,
                .sl = 0,
                .r = 0,
        };
        // 发送EOI信号
        if (interrupt_number >= 0x28) {
            // 如果是从PIC引起的，不仅要给主PIC，还要发送给从PIC，
            // 发送 EOI 信号给从 PIC
            outb(PIC_S_CTRL, *(uint8_t*)&ocw2);
        }
        // 发送 EOI 信号给主 PIC
        outb(PIC_M_CTRL, *(uint8_t*)&ocw2);
    }

}

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

    set_interrupt_descriptor(0, interrupt_handler_0);
    set_interrupt_descriptor(1, interrupt_handler_1);
    set_interrupt_descriptor(2, interrupt_handler_2);
    set_interrupt_descriptor(3, interrupt_handler_3);
    set_interrupt_descriptor(4, interrupt_handler_4);
    set_interrupt_descriptor(5, interrupt_handler_5);
    set_interrupt_descriptor(6, interrupt_handler_6);
    set_interrupt_descriptor(7, interrupt_handler_7);
    set_interrupt_descriptor(8, interrupt_handler_8);
    set_interrupt_descriptor(9, interrupt_handler_9);
    set_interrupt_descriptor(10, interrupt_handler_10);
    set_interrupt_descriptor(11, interrupt_handler_11);
    set_interrupt_descriptor(12, interrupt_handler_12);
    set_interrupt_descriptor(13, interrupt_handler_13);
    set_interrupt_descriptor(14, interrupt_handler_14);
    set_interrupt_descriptor(15, interrupt_handler_15);
    set_interrupt_descriptor(16, interrupt_handler_16);
    set_interrupt_descriptor(17, interrupt_handler_17);
    set_interrupt_descriptor(18, interrupt_handler_18);
    set_interrupt_descriptor(19, interrupt_handler_19);
    set_interrupt_descriptor(20, interrupt_handler_20);
    set_interrupt_descriptor(21, interrupt_handler_21);
    set_interrupt_descriptor(22, interrupt_handler_22);
    set_interrupt_descriptor(23, interrupt_handler_23);
    set_interrupt_descriptor(24, interrupt_handler_24);
    set_interrupt_descriptor(25, interrupt_handler_25);
    set_interrupt_descriptor(26, interrupt_handler_26);
    set_interrupt_descriptor(27, interrupt_handler_27);
    set_interrupt_descriptor(28, interrupt_handler_28);
    set_interrupt_descriptor(29, interrupt_handler_29);
    set_interrupt_descriptor(30, interrupt_handler_30);
    set_interrupt_descriptor(31, interrupt_handler_31);
    set_interrupt_descriptor(32, interrupt_handler_32);
    set_interrupt_descriptor(33, interrupt_handler_33);
    set_interrupt_descriptor(34, interrupt_handler_34);
    set_interrupt_descriptor(35, interrupt_handler_35);
    set_interrupt_descriptor(36, interrupt_handler_36);
    set_interrupt_descriptor(37, interrupt_handler_37);
    set_interrupt_descriptor(38, interrupt_handler_38);
    set_interrupt_descriptor(39, interrupt_handler_39);
    set_interrupt_descriptor(40, interrupt_handler_40);
    set_interrupt_descriptor(41, interrupt_handler_41);
    set_interrupt_descriptor(42, interrupt_handler_42);
    set_interrupt_descriptor(43, interrupt_handler_43);
    set_interrupt_descriptor(44, interrupt_handler_44);
    set_interrupt_descriptor(45, interrupt_handler_45);
    set_interrupt_descriptor(46, interrupt_handler_46);
    set_interrupt_descriptor(47, interrupt_handler_47);


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
