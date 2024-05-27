//
// Created by huangcheng on 2024/5/27.
//

#include "pit8253.h"

/* 把操作的计数器counter_no、读写锁属性rwl、计数器模式counter_mode写入模式控制寄存器并赋予初始值counter_value */
static void frequency_set(uint8_t counter_port,
			  uint8_t counter_no,
			  uint8_t rwl,
			  uint8_t counter_mode,
			  uint16_t counter_value) {
    put_int((uint8_t)counter_value);
    put_char('\n');
    put_int((uint8_t)counter_value >> 8);
    put_char('\n');
/* 往控制字寄存器端口0x43中写入控制字 */
    outb(PIT_CONTROL_PORT, (uint8_t)(counter_no << 6 | rwl << 4 | counter_mode << 1));
/* 先写入counter_value的低8位 */
    outb(counter_port, (uint8_t)counter_value);
/* 再写入counter_value的高8位 */
    outb(counter_port, (uint8_t)counter_value >> 8);
}


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

    CountValue value;
    value.value = COUNTER0_VALUE;
    put_int(value.part.offset_low);
    put_char('\n');
    put_int(value.part.offset_high);
    put_char('\n');

    // 设置计数初始值（分两次写入）
    outb(CONTRER0_PORT, value.part.offset_low);
    outb(CONTRER0_PORT, value.part.offset_high);

}
