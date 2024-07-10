//
// Created by huangcheng on 2024/6/24.
//

#include "../lib/lib_kernel/lib_kernel.h"
#include "../kernel/kernel_page/kernel_page.h"
#include "../kernel/kernel_task/kernel_task.h"
#include "../kernel/kernel_interrupt/kernel_interrupt.h"

// 关于定时器：
// https://developer.arm.com/documentation/ddi0438/i/generic-timer/generic-timer-programmers-model?lang=en
// https://developer.arm.com/documentation/ddi0406/cd/?lang=en

void set_cntfrq(uint32_t value);
void set_cntp_tval(uint32_t value);
void enable_cntp_timer();
void disable_cntp_timer();
uint32_t read_cntpct();

// 用于测试通用中断控制器GIC和中断处理函数是否正常使用
void test();

void kernel_main(void) {
    // u-boot已经给我们设置了管理模式
    // 一进来就已经是管理模式了

    // 页表初始化
    init_paging();

    // 因为覆盖了整个内存区域，所以还可以用u-boot的栈
    // 到这里再切换也没问题

    // 切换栈指针到虚拟地址，这样就可以顺利使用内核栈了
    switch_sp(0xc0007000);
    // 初始化task
    init_multitasking();
    // 初始化内存管理（本来这里应该从环境里面获取内存大小，我懒，直接写死了算了）
    init_memory(0x8000000);
    // 初始化中断管理和GIC
    init_interrupt();

    // 关于定时器的设备树片段：
    // timer {
    //         interrupts = <0x00000001 0x0000000d 0x00000104 0x00000001 0x0000000e 0x00000104 0x00000001 0x0000000b 0x00000104 0x00000001 0x0000000a 0x00000104>;
    //         always-on;
    //         compatible = "arm,armv7-timer";
    // };

    // 解读如下：
    // 这个timer节点定义了4个中断。
    // 每个中断的格式是 <type number> <interrupt number> <flags>，每个中断由3个值组成：
    // 0x00000001 0x0000000d 0x00000104
    // 0x00000001 0x0000000e 0x00000104
    // 0x00000001 0x0000000b 0x00000104
    // 0x00000001 0x0000000a 0x00000104
    // 每个中断的中断号如下：
    // 第一个中断：0x0000000d
    // 第二个中断：0x0000000e
    // 第三个中断：0x0000000b
    // 第四个中断：0x0000000a

    // 总结：
    // 该timer节点定义了4个中断。
    // 中断号分别是0x0d, 0x0e, 0x0b, 和0x0a。
    // 中断标志0x00000104表示这些中断是上升沿触发的。

    // 上升沿触发是一种边缘触发，所以要额外重新配置
    // 但是我试过，这些都没法用
    // 定时器中断id就是30，还是水平触发的

    add_interrupt_handler(30, test, 0);
    enable_gic_irq_interrupt(30);


    // 使用的是硬件定时器
    // 设置计数器，单位是Hz（赫兹，每秒钟计时多少次）
    // 一般都是设置为 1MHz（一百万赫兹）
    set_cntfrq(1000000);
    // 计时器的值，每次操作就-1，减到0就打一次中断
    // 理论上set_cntp_tval(1000000)设置应该是1s，在qemu上实测快得多，差不多是100毫秒
    // 之前x86设置是一秒一百次，所以设置为100000就行了
    set_cntp_tval(100000);
    // 启用定时器
    enable_cntp_timer();

    // 开启IRQ中断
    intr_enable();

    for(;;);
}

// 定义寄存器访问宏
#define CP15_WRITE_REG32(reg, val) \
    __asm__ __volatile__("mcr p15, 0, %0, " reg : : "r"(val))

#define CP15_READ_REG32(reg, val) \
    __asm__ __volatile__("mrc p15, 0, %0, " reg : "=r"(val))

// CP15 寄存器定义
#define CNTFRQ     "c14, c0, 0"  // Counter Frequency Register
#define CNTP_TVAL  "c14, c2, 0"  // Physical Timer Value Register
#define CNTP_CTL   "c14, c2, 1"  // Physical Timer Control Register
#define CNTP_CVAL  "c14, c2, 2"  // Physical Timer Compare Value Register
#define CNTPCT     "c14, c0, 1"  // Physical Count Register

void set_cntfrq(uint32_t value) {
    CP15_WRITE_REG32(CNTFRQ, value);
}

void set_cntp_tval(uint32_t value) {
    CP15_WRITE_REG32(CNTP_TVAL, value);
}

void enable_cntp_timer() {
    uint32_t value;
    CP15_READ_REG32(CNTP_CTL, value);
    value |= 1;  // Set the enable bit
    CP15_WRITE_REG32(CNTP_CTL, value);
}

void disable_cntp_timer() {
    uint32_t value;
    CP15_READ_REG32(CNTP_CTL, value);
    value &= ~1;  // Clear the enable bit
    CP15_WRITE_REG32(CNTP_CTL, value);
}

uint32_t read_cntpct() {
    uint32_t value;
    CP15_READ_REG32(CNTPCT, value);
    return value;
}

// 用于测试通用中断控制器GIC和中断处理函数是否正常使用
void test() {

    // 逻辑代码
    put_str("interrupt test!\n");

    // 停用定时器
    disable_cntp_timer();
    // 设置计时器初始值
    set_cntp_tval(100000);
    // 启用定时器以生成下一次中断
    enable_cntp_timer();
}
