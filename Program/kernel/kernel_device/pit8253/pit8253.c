//
// Created by huangcheng on 2024/5/27.
//

#include "pit8253.h"

void init_pit8253() {
    //frequency_set(CONTRER0_PORT, COUNTER0_NO, READ_WRITE_LATCH, COUNTER_MODE, COUNTER0_VALUE);
    //return;
    // 设置8253的定时周期,也就是发中断的周期
    PIT8253ControlWord controlWord = {
            .bcd = 0,                   // 16位二进制计数
            .mode = COUNTER_MODE,       // 模式2（计数器终端阵列）
            .rwl = READ_WRITE_LATCH,    // 先读/写LSB，然后读/写MSB
            .counter = COUNTER0_NO      // 选择计数器0
    };
    outb(PIT_CONTROL_PORT, *((uint8_t *)&controlWord));
    // 设置计数初始值（分两次写入）
    outb(CONTRER0_PORT, value.part.offset_low);
    outb(CONTRER0_PORT, value.part.offset_high);

}
