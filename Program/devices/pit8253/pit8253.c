//
// Created by huangcheng on 2024/5/27.
//

#include "pit8253.h"

// 注册宏，定义一个驱动结构体，把驱动结构体实例放到驱动段
REGISTER_DRIVER(pit8253_driver){
        .driver_name = "pit8253",
        .init = init_pit8253,
        .exit = exit_pit8253,
        .interrupt_handler = NULL,
        .read = NULL,
        .write = NULL,
};


void init_pit8253() {
    intr_disable();
    put_str("pit8253 initialization.\n");
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
    put_str("pit8253 initialized.\n");
    intr_enable();
}

void exit_pit8253() {
    intr_disable();
    put_str("pit8253 exiting......\n");
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
    put_str("pit8253 has exited.\n");
    intr_enable();
}
