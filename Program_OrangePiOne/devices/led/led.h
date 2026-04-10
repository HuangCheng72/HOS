//
// Created by huangcheng on 2024/8/1.
//

#ifndef HOS_LED_H
#define HOS_LED_H

#include "../../lib/lib_kernel/lib_kernel.h"

// led驱动的read和write函数用到的参数，两个led灯的状态
struct led_status {
    uint32_t green_status;          // 绿色LED，关闭则为0，开启则为1
    uint32_t red_status;            // 红色LED，关闭则为0，开启则为1
};

#endif //HOS_LED_H
