//
// Created by huangcheng on 2024/5/27.
//

#include "pit8253.h"
#include "../../kernel/debug.h"
#include "../../kernel/kernel_task/kernel_task.h"

// 注册宏，定义一个驱动结构体，把驱动结构体实例放到驱动段
REGISTER_DRIVER(pit8253_driver){
    .driver_name = "pit8253",
    .init = init_pit8253,
    .exit = exit_pit8253,
    .irq = 0,
    .irq_interrupt_handler = interrupt_handler_pit8253,
};

#define IRQ0_FREQUENCY	   100
#define INPUT_FREQUENCY	   1193180
#define COUNTER0_VALUE	   INPUT_FREQUENCY / IRQ0_FREQUENCY
#define CONTRER0_PORT	   0x40
#define COUNTER0_NO	   0
#define COUNTER_MODE	   2
#define READ_WRITE_LATCH   3
#define PIT_CONTROL_PORT   0x43

// 定义PIT8253控制字结构体
typedef struct {
    uint8_t bcd       : 1;  // 位0：BCD模式选择，0=16位二进制计数，1=4位BCD计数

    // 位1-3：工作模式选择，具体模式见下表
    // 000：模式0（中断在终端计数）
    // 001：模式1（可编程单次触发）
    // 010：模式2（计数器终端阵列）
    // 011：模式3（方波生成器）
    // 100：模式4（软件触发定时器）
    // 101：模式5（硬件触发定时器）
    // 110：未定义
    // 111：未定义
    uint8_t mode      : 3;

    // 位4-5：读/写计数器的LSB/MSB选择
    // 00：计数器停止（未定义）
    // 01：读/写计数器的最低有效字节（LSB）
    // 10：读/写计数器的最高有效字节（MSB）
    // 11：先读/写最低有效字节，然后读/写最高有效字节（先LSB后MSB）
    uint8_t rwl        : 2;

    // 位6-7：选择计数器
    // 00：选择计数器0
    // 01：选择计数器1
    // 10：选择计数器2
    // 11：读回控制字（不常用）
    uint8_t counter   : 2;
} PIT8253ControlWord;

// 用联合体分离高位低位
typedef union {
    uint16_t value;
    struct Part {
        uint8_t offset_low;
        uint8_t offset_high;
    } part ;
} CountValue ;


uint32_t ticks;          // ticks是内核自中断开启以来总共的嘀嗒数

void init_pit8253() {

    // 设置8253的定时周期,也就是发中断的周期
    PIT8253ControlWord controlWord = {
            .bcd = 0,                   // 16位二进制计数
            .mode = COUNTER_MODE,       // 模式2（计数器终端阵列）
            .rwl = READ_WRITE_LATCH,    // 先读/写LSB，然后读/写MSB
            .counter = COUNTER0_NO      // 选择计数器0
    };
    outb(PIT_CONTROL_PORT, *((uint8_t *)&controlWord));

    CountValue value;
    value.value = COUNTER0_VALUE;
    // 设置计数初始值（分两次写入）
    outb(CONTRER0_PORT, value.part.offset_low);
    outb(CONTRER0_PORT, value.part.offset_high);

}

void exit_pit8253() {

    // 设置8253的定时周期为0为停止模式
    PIT8253ControlWord controlWord = {
            .bcd = 0,                   // 16位二进制计数
            .mode = 0,                  // 模式0（中断在终端计数）
            .rwl = 0,                   // 计数器停止（未定义）
            .counter = COUNTER0_NO      // 选择计数器0
    };
    outb(PIT_CONTROL_PORT, *((uint8_t *)&controlWord));

    // 复位计数器初始值为0
    CountValue value;
    value.value = 0;
    outb(CONTRER0_PORT, value.part.offset_low);
    outb(CONTRER0_PORT, value.part.offset_high);

}

void interrupt_handler_pit8253() {

    struct task* cur_task = running_task();

    cur_task->elapsed_ticks++;	  // 记录此线程占用的cpu时间嘀

    ticks++;	  //从内核第一次处理时间中断后开始至今的滴哒数,内核态和用户态总共的嘀哒数

    if (cur_task->ticks == 0) {	  // 若进程时间片用完就开始调度新的进程上cpu
        task_schedule();
    } else {				  // 将当前进程的时间片-1
        cur_task->ticks--;
    }

}
