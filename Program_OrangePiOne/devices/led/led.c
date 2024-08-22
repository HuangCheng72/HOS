//
// Created by Administrator on 2024/8/1.
//

#include "led.h"

#include "led.h"
#include "../../kernel/kernel_device/kernel_device.h"

// GPIO寄存器定义
#define GPIO_BASE 0x01F02C00
#define GPIO_A_BASE 0x01C20800
#define GPIO_L_BASE 0x01F02C00

#define GPIO_CFG_OFFSET 0x00
#define GPIO_DATA_OFFSET 0x10

// GPIO端口配置和数据寄存器
#define GPIO_CFG(port) ((volatile uint32_t *)(port + GPIO_CFG_OFFSET))
#define GPIO_DATA(port) ((volatile uint32_t *)(port + GPIO_DATA_OFFSET))

static int green_led_status = 0;
static int red_led_status = 0;

void led_init(void) {
    // 配置GPIO L10 为输出模式
    volatile uint32_t *gpio_l_cfg = GPIO_CFG(GPIO_L_BASE);
    *gpio_l_cfg &= ~(0xF << 20);    // 清除第10个引脚的配置
    *gpio_l_cfg |= (1 << 20);       // 设置为输出模式

    // 配置GPIO A15 为输出模式
    volatile uint32_t *gpio_a_cfg = GPIO_CFG(GPIO_A_BASE);
    *gpio_a_cfg &= ~(0xF << 28);    // 清除第15个引脚的配置
    *gpio_a_cfg |= (1 << 28);       // 设置为输出模式

    // 初始化为低电平
    volatile uint32_t *gpio_l_data = GPIO_DATA(GPIO_L_BASE);
    *gpio_l_data &= ~(1 << 10);

    volatile uint32_t *gpio_a_data = GPIO_DATA(GPIO_A_BASE);
    *gpio_a_data &= ~(1 << 15);
}

void led_exit(void) {
    // 关闭LED
    volatile uint32_t *gpio_l_data = GPIO_DATA(GPIO_L_BASE);
    *gpio_l_data &= ~(1 << 10);

    volatile uint32_t *gpio_a_data = GPIO_DATA(GPIO_A_BASE);
    *gpio_a_data &= ~(1 << 15);
}

int32_t led_read(char *args, uint32_t args_size) {
    if(args_size != sizeof(struct led_io_request)) {
        return -1;
    }
    struct led_io_request *request = (struct led_io_request *)args;

    if (request->led == LED_GREEN) {
        request->action = green_led_status;
    } else if (request->led == LED_RED) {
        request->action = red_led_status;
    } else {
        return -1;
    }
    return 0;
}

int32_t led_write(char *args, uint32_t args_size) {
    if(args_size != sizeof(struct led_io_request)) {
        return -1;
    }
    struct led_io_request *request = (struct led_io_request *)args;

    volatile uint32_t *gpio_l_data = GPIO_DATA(GPIO_L_BASE);
    volatile uint32_t *gpio_a_data = GPIO_DATA(GPIO_A_BASE);

    if (request->led == LED_GREEN) {
        if (request->action) {
            *gpio_l_data |= (1 << 10);
            green_led_status = 1;
        } else {
            *gpio_l_data &= ~(1 << 10);
            green_led_status = 0;
        }
    } else if (request->led == LED_RED) {
        if (request->action) {
            *gpio_a_data |= (1 << 15);
            red_led_status = 1;
        } else {
            *gpio_a_data &= ~(1 << 15);
            red_led_status = 0;
        }
    } else {
        return -1;
    }
    return 0;
}

// 注册驱动结构体
REGISTER_DRIVER(led_driver) {
        .driver_name = "led",
        .init = led_init,
        .exit = led_exit,
        .read = led_read,
        .write = led_write,
        .irq = -1, // 没有中断处理
        .irq_interrupt_handler = NULL,
        .need_command_buffer = 0,
        .need_data_buffer = 0,
};
