//
// Created by huangcheng on 2024/5/27.
//

#ifndef HOS_PIT8253_H
#define HOS_PIT8253_H

#include "../../../lib/lib.h"

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

// pit8253初始化函数
void init_pit8253();

#endif //HOS_PIT8253_H
