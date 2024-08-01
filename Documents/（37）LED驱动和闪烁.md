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

kernel.c

```c
//
// Created by huangcheng on 2024/6/24.
//

#include "../lib/lib_kernel/lib_kernel.h"
#include "../kernel/kernel_page/kernel_page.h"
#include "../kernel/kernel_task/kernel_task.h"
#include "../kernel/kernel_memory/kernel_memory.h"
#include "../kernel/kernel_interrupt/kernel_interrupt.h"
#include "../kernel/kernel_device/kernel_device.h"

#include "../devices/console/console.h"

void led_blink_test(void *args);

void kernel_main(void) {
    // 页表初始化
    // 实测，由于u-boot的页表设置了扩展地址和很多东西（缓存缓冲等）
    // 导致必须关掉MMU之后才能进行页表的初始化（要不然直接写不进真正的物理内存）
    // 排查那么多天排除出这么个结果我也是没话说了
    // 我非常想把禁用MMU和启用MMU写入init_paging中，但是这样的话直接跳不回来，会直接进入数据错误异常处理程序
    // 果然虚拟机还是不靠谱

    // 先禁用MMU
    // 获取当前SCTLR寄存器值
    SCTLR_t sctlr;
    get_sctlr(&sctlr);
    // 禁用MMU
    sctlr.M = 0; // 禁用MMU
    sctlr.A = 0; // 禁用对齐检查
    sctlr.C = 0; // 禁用数据缓存
    // 更新SCTLR寄存器
    set_sctlr(&sctlr);

    // 现在开始才能初始化页表
    init_paging();

    // 启用MMU
    get_sctlr(&sctlr);
    sctlr.M = 1; // 启用MMU
    sctlr.A = 1; // 启用对齐检查
    sctlr.C = 1; // 启用数据缓存
    // 更新SCTLR寄存器
    set_sctlr(&sctlr);

    // 加载内核栈
    switch_sp(0xc0007000);
    // 初始化task
    init_multitasking();
    // 初始化内存管理（本来这里应该从环境里面获取内存大小，我懒，直接写死了算了）
    init_memory(0x20000000);
    // 初始化中断管理和GIC
    init_interrupt();
    // 初始化设备驱动管理
    init_all_devices();

    // 这里需要清除一下0xc0008080处的值
    // 搞不明白这里哪来的值，难道是上电初始化不为0？还是u-boot把这地方写了？
    *((uint32_t *)0xc0008080) = 0;

    task_create("led_blink_test", 31, led_blink_test, NULL);

    // 开启IRQ中断
    intr_enable();
    // 允许定时器中断
    enable_gic_irq_interrupt(50);

    for(;;) {
        // 没什么事就让CPU休息
        task_idle();
    }
}

void led_blink_test(void *args) {
    // 获取LED驱动指针
    struct driver *led_driver = get_driver("led");
    if (led_driver == NULL) {
        // 处理未找到驱动的错误
        return;
    }

    // 参数结构体
    struct led_io_request {
        uint32_t led;       // LED类型：绿色LED或红色LED
        uint32_t action;    // 动作：开或关
    };

    struct led_io_request green_led_on = {0, 1};
    struct led_io_request green_led_off = {0, 0};
    struct led_io_request red_led_on = {1, 1};
    struct led_io_request red_led_off = {1, 0};

    for(;;) {
        // 打开绿色LED，关闭红色LED
        device_write(led_driver, (char *)&green_led_on, sizeof(green_led_on));
        device_write(led_driver, (char *)&red_led_off, sizeof(red_led_off));
        for(uint32_t i = 0; i < 8 * UINT16_MAX; i++); // 延时一段时间

        // 关闭绿色LED，打开红色LED
        device_write(led_driver, (char *)&green_led_off, sizeof(green_led_off));
        device_write(led_driver, (char *)&red_led_on, sizeof(red_led_on));
        for(uint32_t i = 0; i < 8 * UINT16_MAX; i++); // 延时一段时间
    }
}

```





运行效果发在b站视频了。