//
// Created by huangcheng on 2024/8/1.
//

#ifndef HOS_LED_H
#define HOS_LED_H

#include "../../lib/lib_kernel/lib_kernel.h"

#define LED_GREEN 0
#define LED_RED 1

// 参数结构体
struct led_io_request {
    uint32_t led;       // LED类型：绿色LED或红色LED
    uint32_t action;    // 动作：开（1）或关（0）
};

#endif //HOS_LED_H
