# （三十七）LED驱动和闪烁

这部分很容易。

devices文件夹下新建led文件夹。

led.h

```c
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
    uint32_t action;    // 动作：开或关
};

int32_t led_read(char *args, uint32_t args_size);
int32_t led_write(char *args, uint32_t args_size);

#endif //HOS_LED_H

```

led.c

```c
//
// Created by huangcheng on 2024/8/1.
//

#include "led.h"

#include "led.h"
#include "../../kernel/kernel_device/kernel_device.h"

// GPIO寄存器定义
#define GPIO_BASE 0x01F02C00
#define GPIO_A_BASE 0x01C20800  //
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

```

led的Makefile

```makefile

# 目录
DEVICE_DIR = ..
LED_DIR = .

# 源文件
LED_C_SRC = $(LED_DIR)/led.c

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra

# 标签标记所有的任务（输出什么文件），方便修改
DEVICE_OBJS = $(DEVICE_DIR)/led.o

.PHONY: all

all: $(DEVICE_OBJS)

# 编出来放到模块文件夹devices去，别放在这里，方便主Makefile找
$(DEVICE_DIR)/led.o: $(LED_C_SRC)
	$(GCC) $(CFLAGS) -c -o $@ $<

# 文件不生成于此，无需清理规则

```

devices的Makefile

```makefile

# 所有设备目录
DEVICE_DIR = .
TIMER_DIR = timer
CONSOLE_DIR = console
LED_DIR = led

# 标签标记所有的任务（输出什么文件），方便修改
DEVICE_OBJS = timer.o console.o led.o

.PHONY: all

all: $(DEVICE_OBJS)

# 执行各子模块编译
timer.o:
	$(MAKE) -C $(TIMER_DIR) GCC=$(GCC) AS=$(AS) MAKE=$(MAKE)

console.o:
	$(MAKE) -C $(CONSOLE_DIR) GCC=$(GCC) AS=$(AS) MAKE=$(MAKE)

led.o:
	$(MAKE) -C $(LED_DIR) GCC=$(GCC) AS=$(AS) MAKE=$(MAKE)

# 清理规则
clean:
	@if exist $(DEVICE_DIR)\\*.o del $(DEVICE_DIR)\\*.o
	@if exist $(DEVICE_DIR)\\*.bin del $(DEVICE_DIR)\\*.bin

```

运行效果发在b站视频了。