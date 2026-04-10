//
// Created by Administrator on 2024/8/1.
//

#include "led.h"

#include "../gpio/gpio.h"

#include "../../kernel/kernel_device/kernel_device.h"

#define LED_GREEN_GPIO_GROUP    GPIO_GROUP_L
#define LED_GREEN_PIN           10
#define LED_RED_GPIO_GROUP      GPIO_GROUP_A
#define LED_RED_PIN             15

void led_init(void) {
    // LED驱动是GPIO驱动套壳，所以不需要初始化，等GPIO初始化
}

void led_exit(void) {
    // LED驱动是GPIO驱动套壳，所以不需要退出，等GPIO退出
}

// read和write操作对象是led_io_request的实例

int32_t led_read(char *data, uint32_t count) {
    if(data == NULL || count != sizeof(struct led_status)) {
        return -1;
    }

    // 获取GPIO驱动指针
    struct driver_descriptor *   gpio_driver = get_driver("gpio");

    if(gpio_driver == NULL) {
        return -1;  // 未找到GPIO驱动
    }
    if(gpio_driver->device_type != 3 || gpio_driver->device_operator == NULL) {
        return -1;  // 不是GPIO驱动，不提供操作集
    }

    struct led_status *status = (struct led_status *)data;

    status->green_status = (uint32_t)(((struct gpio_device_operator *)(gpio_driver->device_operator))->read_pin(LED_GREEN_GPIO_GROUP, LED_GREEN_PIN));

    if(status->green_status > 1) {
        // 等于0或者1，不然就失败
        return -1;
    }

    status->red_status = (uint32_t)(((struct gpio_device_operator *)(gpio_driver->device_operator))->read_pin(LED_RED_GPIO_GROUP, LED_RED_PIN));

    if(status->red_status > 1) {
        // 等于0或者1，不然就失败
        return -1;
    }

    return 0;
}

int32_t led_write(const char *data, uint32_t count) {
    if(data == NULL || count != sizeof(struct led_status)) {
        return -1;
    }

    // 获取GPIO驱动指针
    struct driver_descriptor *   gpio_driver = get_driver("gpio");

    if(gpio_driver == NULL) {
        return -1;  // 未找到GPIO驱动
    }
    if(gpio_driver->device_type != 3 || gpio_driver->device_operator == NULL) {
        return -1;  // 不是GPIO驱动，不提供操作集
    }

    struct led_status *status = (struct led_status *)data;

    // 校验两个参数是否合法
    if((status->green_status > 1) || (status->red_status > 1)) {
        return -1;
    }

    if (
            (((struct gpio_device_operator *)(gpio_driver->device_operator))->write_pin(LED_GREEN_GPIO_GROUP, LED_GREEN_PIN, status->green_status) == -1) ||
            (((struct gpio_device_operator *)(gpio_driver->device_operator))->write_pin(LED_RED_GPIO_GROUP, LED_RED_PIN, status->red_status) == -1)
       ) {
        return -1;
    }

    return 0;
}

// LED设备提供的操作集
struct char_device_operator led_operator = {
        .read = led_read,
        .write = led_write,
        .configure = NULL,
};

// 注册驱动结构体
REGISTER_DRIVER(led_driver) {
        .driver_name = "led",
        .init = led_init,
        .exit = led_exit,
        .irq_descriptors = NULL,
        .irq_count = 0,
        .device_type = 1,
        .device_operator = &led_operator,
};
