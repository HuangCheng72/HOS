//
// Created by huangcheng on 2024/10/30.
//

#include "kernel_delay.h"
#include "../../devices/timer/timer.h"

#define MAX_DELAY_US 8000  // 8ms，超过这个值的延时不在此模块处理范围

// 为什么是8ms，timer0划定的时间片就是10ms，害怕又出什么问题，所以还是留一点余量，8ms作为上限

void kernel_delay(uint32_t us) {
    // 检查延时范围
    if (us > MAX_DELAY_US) {
        // 如果延时超过最大值，直接返回。后续可能结合多任务机制处理更长的延时。
        return;
    }

    // 设置 timer1 的延时
    set_timer1_delay_us(us);

    // 轮询等待直到 timer1 倒计时结束
    while (!is_timer1_done()) {
        // 在此处轮询，不进行其他操作
    }
}
