//
// Created by huangcheng on 2024/7/13.
//

#include "timer.h"

#include "../../kernel/kernel_device/kernel_device.h"
#include "../../kernel/kernel_task/kernel_task.h"

// 注册宏，定义一个驱动结构体，把驱动结构体实例放到驱动段
REGISTER_DRIVER(timer_driver){
        .driver_name = "timer",
        .init = init_timer,
        .exit = exit_timer,
        .irq = 30,
        .trigger_mode = 0,
        .irq_interrupt_handler = interrupt_handler_timer,
};

// 关于定时器：
// https://developer.arm.com/documentation/ddi0438/i/generic-timer/generic-timer-programmers-model?lang=en
// https://developer.arm.com/documentation/ddi0406/cd/?lang=en

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

// timer初始化函数
void init_timer() {

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

    // 使用的是硬件定时器
    // 设置计数器，单位是Hz（赫兹，每秒钟计时多少次）
    // 一般都是设置为 1MHz（一百万赫兹）
    set_cntfrq(1000000);
    // 计时器的值，每次操作就-1，减到0就打一次中断
    // 理论上set_cntp_tval(1000000)设置应该是1s，在qemu上实测快得多，差不多是100毫秒
    // 之前x86设置是一秒一百次，在这里设置成500000就行了，没必要那么快
    set_cntp_tval(500000);
    // 启用定时器
    enable_cntp_timer();
}

// timer退出函数
void exit_timer() {
    // 直接禁用定时器就行了
    disable_cntp_timer();
}

// 初始化
uint32_t total_ticks = 0;

// timer中断处理函数
void interrupt_handler_timer() {
    // 逻辑代码
    struct task* cur_task = running_task();
    cur_task->elapsed_ticks++;	  // 记录此线程占用的cpu时间嘀嗒数

    total_ticks++;                // 总时长增加

    if (cur_task->ticks == 0) {	  // 若任务时间片用完就开始调度新的任务上cpu
        task_schedule();
    } else {				  // 将当前任务的时间片-1
        cur_task->ticks--;
    }

    // 停用定时器
    disable_cntp_timer();
    // 设置计时器初始值
    set_cntp_tval(500000);
    // 启用定时器以生成下一次中断
    enable_cntp_timer();
}
