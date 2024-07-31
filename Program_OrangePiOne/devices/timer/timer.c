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
        .irq = 50,              // 根据Allwinner_H3_Datasheet_v1.2.pdf，page 207，Timer 0的中断号是50
        .trigger_mode = 0,
        .irq_interrupt_handler = interrupt_handler_timer,
};

// 这部分内容来自Allwinner_H3_Datasheet_v1.2.pdf，page 156
#define TIMER_BASE 0x01c20c00

#define TIMER_IRQ_EN        (*(volatile uint32_t *)(TIMER_BASE + 0x00)) // 定时器中断使能寄存器
#define TIMER_IRQ_STA       (*(volatile uint32_t *)(TIMER_BASE + 0x04)) // 定时器状态寄存器

// 定时器0寄存器
#define TIMER0_CTRL         (*(volatile uint32_t *)(TIMER_BASE + 0x10)) // 定时器0控制寄存器
#define TIMER0_INTV_VALUE   (*(volatile uint32_t *)(TIMER_BASE + 0x14)) // 定时器0间隔值寄存器
#define TIMER0_CUR_VALUE    (*(volatile uint32_t *)(TIMER_BASE + 0x18)) // 定时器0当前值寄存器

// 定时器1寄存器（暂不使用，仅供参考）
#define TIMER1_CTRL         (*(volatile uint32_t *)(TIMER_BASE + 0x20)) // 定时器1控制寄存器
#define TIMER1_INTV_VALUE   (*(volatile uint32_t *)(TIMER_BASE + 0x24)) // 定时器1间隔值寄存器
#define TIMER1_CUR_VALUE    (*(volatile uint32_t *)(TIMER_BASE + 0x28)) // 定时器1当前值寄存器

// Allwinner H3的硬件定时器的其他寄存器
// AVS控制寄存器
#define AVS_CNT_CTL         (*(volatile uint32_t *)(TIMER_BASE + 0x80)) // AVS控制寄存器
#define AVS_CNT0            (*(volatile uint32_t *)(TIMER_BASE + 0x84)) // AVS计数器0寄存器
#define AVS_CNT1            (*(volatile uint32_t *)(TIMER_BASE + 0x88)) // AVS计数器1寄存器
#define AVS_CNT_DIV         (*(volatile uint32_t *)(TIMER_BASE + 0x8C)) // AVS分频器寄存器

// 看门狗0寄存器
#define WDOG0_IRQ_EN        (*(volatile uint32_t *)(TIMER_BASE + 0xA0)) // 看门狗0中断使能寄存器
#define WDOG0_IRQ_STA       (*(volatile uint32_t *)(TIMER_BASE + 0xA4)) // 看门狗0状态寄存器
#define WDOG0_CTRL          (*(volatile uint32_t *)(TIMER_BASE + 0xB0)) // 看门狗0控制寄存器
#define WDOG0_CFG           (*(volatile uint32_t *)(TIMER_BASE + 0xB4)) // 看门狗0配置寄存器
#define WDOG0_MODE          (*(volatile uint32_t *)(TIMER_BASE + 0xB8)) // 看门狗0模式寄存器

#define TIMER_CTRL_ENABLE (1 << 0)      // 启用定时器
#define TIMER_CTRL_RELOAD (1 << 1)      // 启用重装载
#define TIMER_CTRL_CLK_SRC (1 << 2)     // 时钟源选择
#define TIMER_CTRL_SINGLE_MODE (1 << 7) // 单次模式

// timer初始化函数
void init_timer() {

    // 关于定时器的设备树片段：
    // timer@01c20c00 {
    //     compatible = "allwinner,sun4i-a10-timer";
    //     reg = <0x01c20c00 0x000000a0>;
    //     interrupts = <0x00000000 0x00000012 0x00000004 0x00000000 0x00000013 0x00000004>;
    //     clocks = <0x00000002>;
    //     status = "okay";
    // };
    // 这个定时器是板子上自带的硬件定时器，这个频率是24MHz

    // 停止定时器0
    TIMER0_CTRL = 0;

    // 设置定时器0的初始计数值，这里设置为一个具体的值，可以根据需要调整
    // 定时器频率是24MHz（我没有分频），也就是一秒钟计24 * 1000000次
    // 我要的中断频率是100Hz，也就是一秒钟中断100次
    // 那么计数值直接就是24 * 1000000 / 100 = 240000
    TIMER0_INTV_VALUE = 240000; // 根据需要调整

    // 清除定时器0的当前计数值
    TIMER0_CUR_VALUE = 0;

    // 启用定时器0中断
    TIMER_IRQ_EN |= (1 << 0);

    // 配置定时器0控制寄存器
    // 使用24M时钟源，启用重装载
    TIMER0_CTRL = TIMER_CTRL_CLK_SRC | TIMER_CTRL_RELOAD;

    // 等待重装载设置完毕
    while ( TIMER0_CTRL & TIMER_CTRL_RELOAD );

    // 启用定时器0
    TIMER0_CTRL |= TIMER_CTRL_ENABLE;
}

// timer退出函数
void exit_timer() {
    // 停止定时器0
    TIMER0_CTRL = 0;

    // 禁用定时器0中断
    TIMER_IRQ_EN = 0;
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

    // 清除定时器0中断状态
    TIMER_IRQ_STA = 0x01;
}
