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
        .irq = 10,
        .irq_interrupt_handler = interrupt_handler_timer,
        .need_data_buffer = 0,
        .need_command_buffer = 0,
        .driver_task_function = NULL,
        .driver_task_function_args = NULL,
};
// 韦东山老师的例程里面，定时器的中断号设定为10，我估计出厂的时候就已经写死了

/*PWM & Timer registers*/
#define	TCFG0		(*(volatile unsigned long *)0x51000000)
#define	TCFG1		(*(volatile unsigned long *)0x51000004)
#define	TCON		(*(volatile unsigned long *)0x51000008)
#define	TCNTB0		(*(volatile unsigned long *)0x5100000c)
#define	TCMPB0		(*(volatile unsigned long *)0x51000010)
#define	TCNTO0		(*(volatile unsigned long *)0x51000014)

// timer初始化函数
void init_timer() {
    TCFG0  = 99;        // 预分频器0 = 99
    TCFG1  = 0x03;      // 选择16分频
    TCNTB0 = 6250;      // 0.1秒钟触发一次中断
    TCON   |= (1<<1);   // 手动更新
    TCON   = 0x09;      // 自动加载，清“手动更新”位，启动定时器0
}

// timer退出函数
void exit_timer() {
    // 没什么需要做的，直接设置为空函数
    return;
}

// 初始化
uint32_t total_ticks = 0;

#define GPFDAT              (*(volatile unsigned long *)0x56000054)

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
}
