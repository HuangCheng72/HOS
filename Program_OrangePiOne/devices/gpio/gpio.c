//
// Created by huangcheng on 2024/8/22.
//

#include "gpio.h"
#include "../../kernel/kernel_device/kernel_device.h"

// 这部分内容位于全志H3技术手册的P 316开始部分
// A到G组是一个大组，归属一个控制器，从P 316到P 344就是讲这一大家子的
// L组是单独的一个，归属另一个控制器，从P 345到P 350

// 各组GPIO地址，每组含有32个IO
#define SUNXI_GPIO_A_BASE 0x01C20800
#define SUNXI_GPIO_B_BASE 0x01C20824
#define SUNXI_GPIO_C_BASE 0x01C20848
#define SUNXI_GPIO_D_BASE 0x01C2086C
#define SUNXI_GPIO_E_BASE 0x01C20890
#define SUNXI_GPIO_F_BASE 0x01C208B4
#define SUNXI_GPIO_G_BASE 0x01C208D8
#define SUNXI_GPIO_L_BASE 0x01F02C00

// GPIO组定义
struct sunxi_gpio {
    volatile uint32_t cfg[4];
    volatile uint32_t dat;
    volatile uint32_t drv[2];
    volatile uint32_t pull[2];
};

static struct sunxi_gpio *get_gpio_group(char group) {
    switch (group) {
        case 'A': return (struct sunxi_gpio *)SUNXI_GPIO_A_BASE;
        case 'B': return (struct sunxi_gpio *)SUNXI_GPIO_B_BASE;
        case 'C': return (struct sunxi_gpio *)SUNXI_GPIO_C_BASE;
        case 'D': return (struct sunxi_gpio *)SUNXI_GPIO_D_BASE;
        case 'E': return (struct sunxi_gpio *)SUNXI_GPIO_E_BASE;
        case 'F': return (struct sunxi_gpio *)SUNXI_GPIO_F_BASE;
        case 'G': return (struct sunxi_gpio *)SUNXI_GPIO_G_BASE;
        case 'L': return (struct sunxi_gpio *)SUNXI_GPIO_L_BASE;
        default: return NULL;
    }
}

// 初始化函数
void gpio_init(void) {
    // GPIO 属于 PIO，总线时钟由 CCU 控制，这里做最小可用初始化
    // 同时会保证 UART0 相关时钟可用，避免串口和GPIO依赖顺序问题
    // CCU_INIT 由驱动管理框架自动运行
    // 所以 GPIO_INIT 这里暂时什么都不用做
}

// 退出函数
void gpio_exit(void) {
    // 不做“清空所有GPIO配置”的破坏性操作，保留原样等待关机
}

// GPIO读入（注意，需要设置上拉电阻和下拉电阻，防止抖动）
int32_t gpio_read(char *args, uint32_t args_size) {
    if (args == NULL || args_size != sizeof(struct gpio_request)) {
        return -1;
    }

    struct gpio_request *request = (struct gpio_request *)args;

    if (request->pin >= 32) {
        return -1;
    }

    struct sunxi_gpio *gpio_group = get_gpio_group(request->group);
    if (gpio_group == NULL) {
        return -1;
    }

    // 检查当前引脚是否为输入模式，否则更改为输入模式
    uint32_t cfg_reg = gpio_group->cfg[request->pin / 8];
    uint32_t shift = (request->pin % 8) * 4;
    uint32_t mode = (cfg_reg >> shift) & 0xf;

    if (mode != 0) {  // 0表示输入模式
        cfg_reg &= ~(0xf << shift);         // 注释，这里是AI纠错的时候发现的，原先用0x7目的也是去清位，但是搞错了，0x7清不到最高位，用0xf才能清到最高位
        gpio_group->cfg[request->pin / 8] = cfg_reg;
    }

    // 读取引脚电平值
    request->value = (gpio_group->dat >> request->pin) & 0x1;

    return 0;
}

// GPIO写出
int32_t gpio_write(char *args, uint32_t args_size) {
    if (args == NULL || args_size != sizeof(struct gpio_request)) {
        return -1;
    }

    struct gpio_request *request = (struct gpio_request *)args;

    if (request->pin >= 32) {
        return -1;
    }

    struct sunxi_gpio *gpio_group = get_gpio_group(request->group);
    if (gpio_group == NULL) {
        return -1;
    }

    // 检查当前引脚是否为输出模式，否则更改为输出模式
    uint32_t cfg_reg = gpio_group->cfg[request->pin / 8];
    uint32_t shift = (request->pin % 8) * 4;
    uint32_t mode = (cfg_reg >> shift) & 0xF;

    if (mode != 1) {  // 1表示输出模式
        cfg_reg &= ~(0xf << shift);         // 注释，这里是AI纠错的时候发现的，原先用0x7目的也是去清位，但是搞错了，0x7清不到最高位，用0xf才能清到最高位
        cfg_reg |= (0x1 << shift);
        gpio_group->cfg[request->pin / 8] = cfg_reg;
    }

    // 设置引脚电平值
    if (request->value) {
        gpio_group->dat |= (1 << request->pin);
    } else {
        gpio_group->dat &= ~(1 << request->pin);
    }

    return 0;
}

// 注册驱动结构体
REGISTER_DRIVER(gpio_driver) {
        .driver_name = "gpio",
        .init = gpio_init,
        .exit = gpio_exit,
        .read = gpio_read,
        .write = gpio_write,
        .irq = -1, // 没有中断处理
        .irq_interrupt_handler = NULL,
        .need_command_buffer = 0,
        .need_data_buffer = 0,
};
