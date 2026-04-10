//
// Created by huangcheng on 2024/8/22.
//

#ifndef HOS_GPIO_H
#define HOS_GPIO_H

#include "../../lib/lib_kernel/lib_kernel.h"

// 这个设备GPIO的相关信息

// GPIO 组枚举
// 这里枚举的是设备当前支持的 GPIO 组
typedef enum {
    GPIO_GROUP_A = 0,     // PA 组
    GPIO_GROUP_C = 1,     // PC 组
    GPIO_GROUP_D = 2,     // PD 组
    GPIO_GROUP_E = 3,     // PE 组
    GPIO_GROUP_F = 4,     // PF 组
    GPIO_GROUP_G = 5,     // PG 组
    GPIO_GROUP_L = 6,     // PL 组

    GPIO_GROUP_COUNT       // GPIO 组数量
} gpio_group_t;

// GPIO 引脚功能模式
// 这里不强行只限制输入/输出，而是允许你后面扩展复用功能
typedef enum {
    GPIO_PIN_FUNC_INPUT      = 0,   // 输入
    GPIO_PIN_FUNC_OUTPUT     = 1,   // 输出
    GPIO_PIN_FUNC_ALT2       = 2,   // 复用功能2
    GPIO_PIN_FUNC_ALT3       = 3,   // 复用功能3
    GPIO_PIN_FUNC_ALT4       = 4,   // 复用功能4
    GPIO_PIN_FUNC_ALT5       = 5,   // 复用功能5
    GPIO_PIN_FUNC_EINT       = 6,   // 外部中断
    GPIO_PIN_FUNC_DISABLE    = 7    // IO Disable
} gpio_pin_function_t;


// GPIO 上下拉配置
typedef enum {
    GPIO_PULL_DISABLE = 0,
    GPIO_PULL_UP      = 1,
    GPIO_PULL_DOWN    = 2
} gpio_pull_t;


// GPIO 驱动能力等级
typedef enum {
    GPIO_DRIVE_LEVEL0 = 0,
    GPIO_DRIVE_LEVEL1 = 1,
    GPIO_DRIVE_LEVEL2 = 2,
    GPIO_DRIVE_LEVEL3 = 3
} gpio_drive_level_t;


// 单个 pin 的配置结构体（传给configure函数的参数，configure函数按照这个结构体来解读意义）
struct gpio_config {
    gpio_group_t group;                 // GPIO 组
    uint32_t pin;                       // 针脚号

    gpio_pin_function_t function;       // 功能模式
    gpio_pull_t pull;                   // 上下拉配置
    gpio_drive_level_t drive_level;     // 驱动能力等级

                                        // init_value:
                                        //   -1 = 不改输出值
                                        //    0 = 配置后输出低电平
                                        //    1 = 配置后输出高电平
    int32_t init_value;
};

#endif //HOS_GPIO_H
