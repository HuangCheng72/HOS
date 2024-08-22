# （三十八）LED驱动换成GPIO驱动



最近精神不好，就没打代码也没更新。

这部分是我尝试直接从u-boot中移植驱动。

u-boot仓库为迅龙官方的：https://github.com/orangepi-xunlong/OrangePiH3_uboot/

只移植了GPIO，是因为我发现其他的设备都需要先把时钟搞清楚，而我实在没精力鼓捣时钟了，等以后有空再说吧。



## 0. us级的delay实现

delay.asm

```assembly
.section .text
.global delay

// 举个例子，如果CPU的频率是1MHz，也就是百万赫兹正好，就说明它一秒钟可以执行一百万次
// 一条指令就要执行一次

delay:
    // 纳秒值由r0传入，使用r1来实际计次
    // 循环体是两条语句，这两条语句都是单次执行，所以如果要正好计算一秒钟，那么r1应该设置为50万次（一百万除以2）

    // 这个CPU核心默认的的标称频率是1.2GHz（1200MHz），如果没有任何手动限制，应该都是这个频率
    // 那么一秒钟之内它应该能执行1200 * 1000 * 1000条语句
    // 1 ns = 10^(-9) s
    // 去掉九个0，1 ns之内能执行1.2条语句

    // r1的值是计次次数，要达到的结果是 r1 * 2 + （计算r1过程语句数量） + （最后一句bx） = r0 * 1.2
    // r1 = (r0 * 1.2 - （计算r1过程语句数量） - （最后一句bx） ) / 2

    // 这个实在是太难精确计算了，死马当活马医，只要数量级正确就行
    // 约等于1GHz，那么就是1 ns一条，直接把1.2的系数删掉了
    // 同时忽略掉计算过程语句数量和最后一句bx

    // 直接下面这样解决问题

    mov r1, r0          // 把r0的值赋予r1
    lsr r1, r1, #1      // 把r1的值右移一位，即除以2
delay_loop:
    sub r1, r1, #1      // 将r1减去1
    bne delay_loop      // 如果计算结果为0，cpsr的运算结果为0标志位会更新，所以可以直接判断，如果不为0就跳回去
return_caller:
    bx  lr              // 执行完毕返回调用者

```

lib_kernel.h中添加

```c
// 延迟给定纳秒数
extern void delay(uint32_t us);
```



## 1. 代码

这部分代码的直接来源是：

[OrangePiH3_uboot/drivers/gpio/sunxi_gpio.c at master · orangepi-xunlong/OrangePiH3_uboot (github.com)](https://github.com/orangepi-xunlong/OrangePiH3_uboot/blob/master/drivers/gpio/sunxi_gpio.c)

移植过程相当于我将其填到我的驱动管理框架里面。

gpio.h

```c
//
// Created by huangcheng on 2024/8/22.
//

#ifndef HOS_GPIO_H
#define HOS_GPIO_H

#include "../../lib/lib_kernel/lib_kernel.h"

struct gpio_request {
    char group;     // 组号，如 'A', 'B', 'C', ... 'L'
    uint8_t pin;    // 针脚编号（0-31）
    uint8_t value;  // 用于写操作的电平值，0表示低电平，1表示高电平
};

int32_t gpio_read(char *args, uint32_t args_size);
int32_t gpio_write(char *args, uint32_t args_size);

#endif //HOS_GPIO_H

```

gpio.c

```c
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

```



## 2. 实际测试

kernel.c中的led_blink_test任务函数修改如下：

```c
void led_blink_test(void *args) {
    // 获取GPIO驱动指针
    struct driver *gpio_driver = get_driver("gpio");
    if (gpio_driver == NULL) {
        // 处理未找到驱动的错误
        return;
    }

    // 参数结构体
    struct gpio_request {
        char group;     // 组号，如 'A', 'B', 'C', ... 'L'
        uint8_t pin;    // 针脚编号（0-31）
        uint8_t value;  // 用于写操作的电平值，0表示低电平，1表示高电平
    };

    struct gpio_request green_led_on = {'L',10, 1};     // GPIO L10 绿色LED 开
    struct gpio_request green_led_off = {'L',10, 0};    // GPIO L10 绿色LED 关
    struct gpio_request red_led_on = {'A',15, 1};       // GPIO A15 红色LED 开
    struct gpio_request red_led_off = {'A',15, 0};      // GPIO A15 红色LED 关

    for(;;) {
        // 打开绿色LED，关闭红色LED
        device_write(gpio_driver, (char *)&green_led_on, sizeof(green_led_on));
        device_write(gpio_driver, (char *)&red_led_off, sizeof(red_led_off));
        for(uint32_t i = 0; i < 8 * UINT16_MAX; i++); // 延时一段时间

        // 关闭绿色LED，打开红色LED
        device_write(gpio_driver, (char *)&green_led_off, sizeof(green_led_off));
        device_write(gpio_driver, (char *)&red_led_on, sizeof(red_led_on));
        for(uint32_t i = 0; i < 8 * UINT16_MAX; i++); // 延时一段时间
    }
}

```

编译运行上机，运行之后红绿LED也一样闪烁。



写这个就相当于留个思路吧。从u-boot移植看起来是目前比较可行的路径，除非对照着数据手册一个个驱动自己写。可以直接把代码搬过来，然后调用，只要功能切割的粒度够小，总能拼凑出想要的功能。

该进行其他工作了，我该学学怎么用Linux开发驱动了。

