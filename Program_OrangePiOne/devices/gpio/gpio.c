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

// 初始化函数
void gpio_init(void) {
    // u-boot已经初始化了，别乱动，串口通信也是过GPIO的，乱动就没法用串口通信了
    // 这部分如果要做初始化，参考如下代码

    /*
    struct sunxi_gpio *gpio_group;

    // 处理 A 到 G 组
    for (int group = 0; group < 7; group++) {
        gpio_group = (struct sunxi_gpio *)(SUNXI_GPIO_A_BASE + group * 0x24);

        // 配置所有引脚为输出模式
        for (int i = 0; i < 4; i++) {           // 每个寄存器由4位配置
            gpio_group->cfg[i] = 0x11111111;    // 4 个 cfg 寄存器，每个引脚都配置为输出模式
        }

        // 设置所有引脚为低电平
        gpio_group->dat = 0x00000000;
    }

    // 处理 L 组
    gpio_group = (struct sunxi_gpio *)SUNXI_GPIO_L_BASE;

    // 配置所有引脚为输出模式
    for (int i = 0; i < 4; i++) {
        gpio_group->cfg[i] = 0x11111111;        // 4 个 cfg 寄存器，每个引脚都配置为输出模式
    }

    // 设置所有引脚为低电平
    gpio_group->dat = 0x00000000;

     */

    // 初始化为输入模式的话，要设置上拉电阻或者下拉电阻，我这边暂时用不到就没设置
}

// 退出函数
void gpio_exit(void) {
    struct sunxi_gpio *gpio_group;

    // 处理 A 到 G 组
    for (int group = 0; group < 7; group++) {
        gpio_group = (struct sunxi_gpio *)(SUNXI_GPIO_A_BASE + group * 0x24);

        // 清除所有配置，恢复默认状态
        for (int i = 0; i < 4; i++) {
            gpio_group->cfg[i] = 0x00000000;    // 清除配置
        }
    }

    // 处理 L 组
    gpio_group = (struct sunxi_gpio *)SUNXI_GPIO_L_BASE;

    // 清除所有配置，恢复默认状态
    for (int i = 0; i < 4; i++) {
        gpio_group->cfg[i] = 0x00000000;        // 清除配置
    }
}

// GPIO读入（注意，需要设置上拉电阻和下拉电阻，防止抖动）
int32_t gpio_read(char *args, uint32_t args_size) {
    if (args_size != sizeof(struct gpio_request)) {
        return -1;
    }

    struct gpio_request *request = (struct gpio_request *)args;

    // 确定 GPIO 组的基地址
    uint32_t group_base;
    switch (request->group) {
        case 'A': group_base = SUNXI_GPIO_A_BASE; break;
        case 'B': group_base = SUNXI_GPIO_B_BASE; break;
        case 'C': group_base = SUNXI_GPIO_C_BASE; break;
        case 'D': group_base = SUNXI_GPIO_D_BASE; break;
        case 'E': group_base = SUNXI_GPIO_E_BASE; break;
        case 'F': group_base = SUNXI_GPIO_F_BASE; break;
        case 'G': group_base = SUNXI_GPIO_G_BASE; break;
        case 'L': group_base = SUNXI_GPIO_L_BASE; break;
        default: return -1; // 非法的组号
    }

    struct sunxi_gpio *gpio_group = (struct sunxi_gpio *)group_base;

    // 检查当前引脚是否为输入模式，否则更改为输入模式
    uint32_t cfg_reg = gpio_group->cfg[request->pin / 8];
    int shift = (request->pin % 8) * 4;
    uint32_t mode = (cfg_reg >> shift) & 0x7;

    if (mode != 0) {  // 0表示输入模式
        cfg_reg &= ~(0x7 << shift);
        gpio_group->cfg[request->pin / 8] = cfg_reg;
    }

    // 读取引脚电平值
    request->value = (gpio_group->dat >> request->pin) & 0x1;

    return 0;
}

// GPIO写出
int32_t gpio_write(char *args, uint32_t args_size) {
    if (args_size != sizeof(struct gpio_request)) {
        return -1;
    }

    struct gpio_request *request = (struct gpio_request *)args;

    // 确定 GPIO 组的基地址
    uint32_t group_base;
    switch (request->group) {
        case 'A': group_base = SUNXI_GPIO_A_BASE; break;
        case 'B': group_base = SUNXI_GPIO_B_BASE; break;
        case 'C': group_base = SUNXI_GPIO_C_BASE; break;
        case 'D': group_base = SUNXI_GPIO_D_BASE; break;
        case 'E': group_base = SUNXI_GPIO_E_BASE; break;
        case 'F': group_base = SUNXI_GPIO_F_BASE; break;
        case 'G': group_base = SUNXI_GPIO_G_BASE; break;
        case 'L': group_base = SUNXI_GPIO_L_BASE; break;
        default: return -1; // 非法的组号
    }

    struct sunxi_gpio *gpio_group = (struct sunxi_gpio *)group_base;

    // 检查当前引脚是否为输出模式，否则更改为输出模式
    uint32_t cfg_reg = gpio_group->cfg[request->pin / 8];
    int shift = (request->pin % 8) * 4;
    uint32_t mode = (cfg_reg >> shift) & 0x7;

    if (mode != 1) {  // 1表示输出模式
        cfg_reg &= ~(0x7 << shift);
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
