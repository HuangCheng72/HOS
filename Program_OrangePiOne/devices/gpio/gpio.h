//
// Created by huangcheng on 2024/8/22.
//

#ifndef HOS_GPIO_H
#define HOS_GPIO_H

#include "../../lib/lib_kernel/lib_kernel.h"

struct gpio_request {
    char group;     // 组号，只有这几个：A、C、D、E、F、G、L
    uint8_t pin;    // 针脚编号（0-31）
    uint8_t value;  // 用于写操作的电平值，0表示低电平，1表示高电平
};

#endif //HOS_GPIO_H
