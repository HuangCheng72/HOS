//
// Created by Administrator on 2024/8/1.
//

#include "led.h"

#include "../gpio/gpio.h"

#include "../../kernel/kernel_device/kernel_device.h"

void led_init(void) {
    // LED驱动是GPIO驱动套壳，所以不需要初始化，等GPIO初始化
}

void led_exit(void) {
    // LED驱动是GPIO驱动套壳，所以不需要退出，等GPIO退出
}

// read和write操作对象是led_io_request的实例

int32_t led_read(char *args, uint32_t args_size) {
    if(args == NULL || args_size != sizeof(struct led_io_request)) {
        return -1;
    }

    // 获取GPIO驱动指针
    struct driver *gpio_driver = get_driver("gpio");
    if (gpio_driver == NULL) {
        // 未找到GPIO驱动
        return -1;
    }

    struct led_io_request *request = (struct led_io_request *)args;

    struct gpio_request led_status = {};     // GPIO请求，绿色LED是L组 pin10，红色LED是A组 pin15

    if (request->led == LED_GREEN) {
        led_status.group = 'L';
        led_status.pin = 10;
        led_status.value = 0;

        device_read(gpio_driver, (char*)(&led_status), sizeof(led_status));

        request->action = led_status.value;
    } else if (request->led == LED_RED) {
        led_status.group = 'A';
        led_status.pin = 15;
        led_status.value = 0;

        device_read(gpio_driver, (char*)(&led_status), sizeof(led_status));

        request->action = led_status.value;
    } else {
        return -1;
    }
    return 0;
}

int32_t led_write(char *args, uint32_t args_size) {
    if(args == NULL || args_size != sizeof(struct led_io_request)) {
        return -1;
    }

    // 获取GPIO驱动指针
    struct driver *gpio_driver = get_driver("gpio");
    if (gpio_driver == NULL) {
        // 未找到GPIO驱动
        return -1;
    }

    struct led_io_request *request = (struct led_io_request *)args;

    // 四个状态

    struct gpio_request led_status = {};     // GPIO请求，绿色LED是L组 pin10，红色LED是A组 pin15

    if (request->led == LED_GREEN) {
        if (request->action == 0 || request->action == 1) {
            led_status.group = 'L';
            led_status.pin = 10;
            led_status.value = request->action;

            device_write(gpio_driver, (char *)&led_status, sizeof(led_status));
        }
        else {
            return -1;
        }
    } else if (request->led == LED_RED) {
        if (request->action == 0 || request->action == 1) {
            led_status.group = 'A';
            led_status.pin = 15;
            led_status.value = request->action;

            device_write(gpio_driver, (char *)&led_status, sizeof(led_status));
        }
        else {
            return -1;
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
