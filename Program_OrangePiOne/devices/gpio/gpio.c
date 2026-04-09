//
// Created by huangcheng on 2024/8/22.
//

#include "gpio.h"
#include "gpio_datasheet.h"
#include "../../kernel/kernel_device/kernel_device.h"

// 虽然 gpio_datasheet.h 提供了数据手册里面的完整结构化信息
// 但是这种组织形式还是不直观
// 在这里进行符合阅读习惯的重新整合

// 以下结构，虽然结构体名字不一样，实际上结构是完全一样的，只是每组管理的针脚数量不一致

// 每个 Port 寄存器块步长：0x24

// 各组GPIO地址，每组含有32个IO
#define GPIO_A_BASE     PIO_BASE_ADDR + ((0) * PIO_PORT_STRIDE)

// A组GPIO组内的结构
struct gpio_group_A {
    PA_CFG0_REG_t cfg0;     // A组设置寄存器0，管理0到7号针脚
    PA_CFG1_REG_t cfg1;     // A组设置寄存器1，管理8到15号针脚
    PA_CFG2_REG_t cfg2;     // A组设置寄存器2，管理16到21号针脚，剩余保留
    PA_CFG3_REG_t cfg3;     // 目前没有那么多，这部分作为保留

    PA_DATA_REG_t dat;      // A组的数据寄存器

    PA_DRV0_REG_t drv0;     // A组的驱动能力寄存器0，管理0到15号针脚
    PA_DRV1_REG_t drv1;     // A组的驱动能力寄存器1，管理16到21号针脚，其余保留

    PA_PULL0_REG_t pull0;   // A组的上下拉寄存器0，管理0到15号针脚
    PA_PULL1_REG_t pull1;   // A组的上下拉寄存器1，管理16到21号针脚，剩余保留

};

#define GPIO_C_BASE     PIO_BASE_ADDR + ((1) * PIO_PORT_STRIDE)

// C组GPIO组内的结构
struct gpio_group_C {
    PC_CFG0_REG_t cfg0;     // C组设置寄存器0，管理0到7号针脚
    PC_CFG1_REG_t cfg1;     // C组设置寄存器1，管理8到15号针脚
    PC_CFG2_REG_t cfg2;     // C组设置寄存器2，管理16号针脚，剩余保留
    PC_CFG3_REG_t cfg3;     // 目前没有那么多，这部分作为保留

    PC_DATA_REG_t dat;      // C组的数据寄存器

    PC_DRV0_REG_t drv0;     // C组的驱动能力寄存器0，管理0到15号针脚
    PC_DRV1_REG_t drv1;     // C组的驱动能力寄存器1，管理16号针脚，其余保留

    PC_PULL0_REG_t pull0;   // C组的上下拉寄存器0，管理0到15号针脚
    PC_PULL1_REG_t pull1;   // C组的上下拉寄存器1，管理16号针脚，剩余保留

};

#define GPIO_D_BASE     PIO_BASE_ADDR + ((2) * PIO_PORT_STRIDE)

// D组GPIO组内的结构
struct gpio_group_D {
    PD_CFG0_REG_t cfg0;     // D组设置寄存器0，管理0到7号针脚
    PD_CFG1_REG_t cfg1;     // D组设置寄存器1，管理8到15号针脚
    PD_CFG2_REG_t cfg2;     // D组设置寄存器2，管理16到17号针脚，剩余保留
    PD_CFG3_REG_t cfg3;     // 目前没有那么多，这部分作为保留

    PD_DATA_REG_t dat;      // D组的数据寄存器

    PD_DRV0_REG_t drv0;     // D组的驱动能力寄存器0，管理0到15号针脚
    PD_DRV1_REG_t drv1;     // D组的驱动能力寄存器1，管理16到17号针脚，其余保留

    PD_PULL0_REG_t pull0;   // D组的上下拉寄存器0，管理0到15号针脚
    PD_PULL1_REG_t pull1;   // D组的上下拉寄存器1，管理16到17号针脚，剩余保留

};

#define GPIO_E_BASE     PIO_BASE_ADDR + ((3) * PIO_PORT_STRIDE)

// E组GPIO组内的结构
struct gpio_group_E {
    PE_CFG0_REG_t cfg0;     // E组设置寄存器0，管理0到7号针脚
    PE_CFG1_REG_t cfg1;     // E组设置寄存器1，管理8到15号针脚
    PE_CFG2_REG_t cfg2;     // 目前没有那么多，这部分作为保留
    PE_CFG3_REG_t cfg3;     // 目前没有那么多，这部分作为保留

    PE_DATA_REG_t dat;      // E组的数据寄存器

    PE_DRV0_REG_t drv0;     // E组的驱动能力寄存器0，管理0到15号针脚
    PE_DRV1_REG_t drv1;     // 这部分保留

    PE_PULL0_REG_t pull0;   // E组的上下拉寄存器0，管理0到15号针脚
    PE_PULL1_REG_t pull1;   // 这部分保留

};

#define GPIO_F_BASE     PIO_BASE_ADDR + ((4) * PIO_PORT_STRIDE)

// F组GPIO组内的结构
struct gpio_group_F {
    PF_CFG0_REG_t cfg0;     // F组设置寄存器0，管理0到6号针脚，剩余保留
    PF_CFG1_REG_t cfg1;     // 目前没有那么多，这部分作为保留
    PF_CFG2_REG_t cfg2;     // 目前没有那么多，这部分作为保留
    PF_CFG3_REG_t cfg3;     // 目前没有那么多，这部分作为保留

    PF_DATA_REG_t dat;      // F组的数据寄存器

    PF_DRV0_REG_t drv0;     // F组的驱动能力寄存器0，管理0到6号针脚，剩余保留
    PF_DRV1_REG_t drv1;     // 保留

    PF_PULL0_REG_t pull0;   // F组的上下拉寄存器0，管理0到6号针脚，剩余保留
    PF_PULL1_REG_t pull1;   // 保留

};

#define GPIO_G_BASE     PIO_BASE_ADDR + ((5) * PIO_PORT_STRIDE)

// G组GPIO组内的结构
struct gpio_group_G {
    PG_CFG0_REG_t cfg0;     // G组设置寄存器0，管理0到7号针脚
    PG_CFG1_REG_t cfg1;     // G组设置寄存器1，管理8到13号针脚
    PG_CFG2_REG_t cfg2;     // 目前没有那么多，这部分作为保留
    PG_CFG3_REG_t cfg3;     // 目前没有那么多，这部分作为保留

    PG_DATA_REG_t dat;      // G组的数据寄存器

    PG_DRV0_REG_t drv0;     // G组的驱动能力寄存器0，管理0到13号针脚，剩余保留
    PG_DRV1_REG_t drv1;     // 保留

    PG_PULL0_REG_t pull0;   // G组的上下拉寄存器0，管理0到13号针脚，剩余保留
    PG_PULL1_REG_t pull1;   // 保留

};

#define GPIO_L_BASE     R_PIO_BASE_ADDR

// L组GPIO组内的结构
struct gpio_group_L {
    PL_CFG0_REG_t cfg0;     // L组设置寄存器0，管理0到7号针脚
    PL_CFG1_REG_t cfg1;     // L组设置寄存器1，管理8到11号针脚，剩余保留
    PL_CFG2_REG_t cfg2;     // 目前没有那么多，这部分作为保留
    PL_CFG3_REG_t cfg3;     // 目前没有那么多，这部分作为保留

    PL_DATA_REG_t dat;      // L组的数据寄存器

    PL_DRV0_REG_t drv0;     // L组的驱动能力寄存器0，管理0到11号针脚，剩余保留
    PL_DRV1_REG_t drv1;     // 保留

    PL_PULL0_REG_t pull0;   // L组的上下拉寄存器0，管理0到11号针脚，剩余保留
    PL_PULL1_REG_t pull1;   // 保留

};

// A、G、L三组有外部中断配置，是外部中断相关配置的结构体

#define GPIO_A_EINT_BASE     GPIO_A_BASE + PA_EINT_CFG0_REG_OFFSET

// A组GPIO外部中断配置的结构
struct gpio_eint_group_A {
    PA_EINT_CFG0_REG_t cfg0;     // A组外部中断设置寄存器0，管理外部中断0到7
    PA_EINT_CFG1_REG_t cfg1;     // A组外部中断设置寄存器1，管理外部中断8到15
    PA_EINT_CFG2_REG_t cfg2;     // A组外部中断设置寄存器2，管理外部中断16到21，其余保留
    PA_EINT_CFG3_REG_t cfg3;     // 目前没有那么多，这部分作为保留

    PA_EINT_CTL_REG_t       ctl;        // A组外部中断控制寄存器

    PA_EINT_STATUS_REG_t    status;     // A组外部中断状态寄存器

    PA_EINT_DEB_REG_t    deb;           // A组外部中断去抖动寄存器

};

#define GPIO_G_EINT_BASE     GPIO_G_BASE + PG_EINT_CFG0_REG_OFFSET

// G组GPIO外部中断配置的结构
struct gpio_eint_group_G {
    PG_EINT_CFG0_REG_t cfg0;     // G组外部中断设置寄存器0，管理外部中断0到7
    PG_EINT_CFG1_REG_t cfg1;     // G组外部中断设置寄存器1，管理外部中断8到13，其余保留
    PG_EINT_CFG2_REG_t cfg2;     // 目前没有那么多，这部分作为保留
    PG_EINT_CFG3_REG_t cfg3;     // 目前没有那么多，这部分作为保留

    PG_EINT_CTL_REG_t       ctl;        // G组外部中断控制寄存器

    PG_EINT_STATUS_REG_t    status;     // G组外部中断状态寄存器

    PG_EINT_DEB_REG_t    deb;           // G组外部中断去抖动寄存器

};

#define GPIO_L_EINT_BASE     GPIO_L_BASE + PL_EINT_CFG0_REG_OFFSET

// G组GPIO外部中断配置的结构
struct gpio_eint_group_L {
    PL_EINT_CFG0_REG_t cfg0;     // L组外部中断设置寄存器0，管理外部中断0到7
    PL_EINT_CFG1_REG_t cfg1;     // L组外部中断设置寄存器1，管理外部中断8到11，其余保留
    PL_EINT_CFG2_REG_t cfg2;     // 目前没有那么多，这部分作为保留
    PL_EINT_CFG3_REG_t cfg3;     // 目前没有那么多，这部分作为保留

    PL_EINT_CTL_REG_t       ctl;        // L组外部中断控制寄存器

    PL_EINT_STATUS_REG_t    status;     // L组外部中断状态寄存器

    PL_EINT_DEB_REG_t    deb;           // L组外部中断去抖动寄存器

};


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

    // 因为结构都一样，可以直接调整为统一的一个结构体指针来编码
    // 这里选择A组的结构体

    struct gpio_group_A * gpio_ptr = NULL; // 操作的GPIO的指针

    if (request->group == 'A') {
        if (request->pin > 21) {
            // A组只有0到21号针脚
            return -1;
        }
        gpio_ptr = (struct gpio_group_A *)GPIO_A_BASE;
    }
    if (request->group == 'C') {
        if (request->pin > 16) {
            // C组只有0到16号针脚
            return -1;
        }
        gpio_ptr = (struct gpio_group_A *)GPIO_C_BASE;
    }
    if (request->group == 'D') {
        if (request->pin > 17) {
            // D组只有0到17号针脚
            return -1;
        }
        gpio_ptr = (struct gpio_group_A *)GPIO_D_BASE;
    }
    if (request->group == 'E') {
        if (request->pin > 15) {
            // E组只有0到15号针脚
            return -1;
        }
        gpio_ptr = (struct gpio_group_A *)GPIO_E_BASE;
    }
    if (request->group == 'F') {
        if (request->pin > 6) {
            // F组只有0到6号针脚
            return -1;
        }
        gpio_ptr = (struct gpio_group_A *)GPIO_F_BASE;
    }
    if (request->group == 'G') {
        if (request->pin > 13) {
            // G组只有0到13号针脚
            return -1;
        }
        gpio_ptr = (struct gpio_group_A *)GPIO_G_BASE;
    }
    if (request->group == 'L') {
        if (request->pin > 11) {
            // L组只有0到11号针脚
            return -1;
        }
        gpio_ptr = (struct gpio_group_A *)GPIO_L_BASE;
    }

    if(gpio_ptr == NULL) {
        return -1;  // 输入的GPIO组号非法
    }

    // 检查当前引脚是否为输入模式，否则更改为输入模式

    // 为了方便，懒得判断了，直接统一修正为输入模式

    // 我之所以要这么做也是因为我特别厌恶位运算，没办法，我是真的对位运算很不喜欢，我宁愿这样复制粘贴都不想写位运算


    if (request->pin == 0) {
        gpio_ptr->cfg0.PA0_SELECT = 0;
    }
    if (request->pin == 1) {
        gpio_ptr->cfg0.PA1_SELECT = 0;
    }
    if (request->pin == 2) {
        gpio_ptr->cfg0.PA2_SELECT = 0;
    }
    if (request->pin == 3) {
        gpio_ptr->cfg0.PA3_SELECT = 0;
    }
    if (request->pin == 4) {
        gpio_ptr->cfg0.PA4_SELECT = 0;
    }
    if (request->pin == 5) {
        gpio_ptr->cfg0.PA5_SELECT = 0;
    }
    if (request->pin == 6) {
        gpio_ptr->cfg0.PA6_SELECT = 0;
    }
    if (request->pin == 7) {
        gpio_ptr->cfg0.PA7_SELECT = 0;
    }
    if (request->pin == 8) {
        gpio_ptr->cfg1.PA8_SELECT = 0;
    }
    if (request->pin == 9) {
        gpio_ptr->cfg1.PA9_SELECT = 0;
    }
    if (request->pin == 10) {
        gpio_ptr->cfg1.PA10_SELECT = 0;
    }
    if (request->pin == 11) {
        gpio_ptr->cfg1.PA11_SELECT = 0;
    }
    if (request->pin == 12) {
        gpio_ptr->cfg1.PA12_SELECT = 0;
    }
    if (request->pin == 13) {
        gpio_ptr->cfg1.PA13_SELECT = 0;
    }
    if (request->pin == 14) {
        gpio_ptr->cfg1.PA14_SELECT = 0;
    }
    if (request->pin == 15) {
        gpio_ptr->cfg1.PA15_SELECT = 0;
    }
    if (request->pin == 16) {
        gpio_ptr->cfg2.PA16_SELECT = 0;
    }
    if (request->pin == 17) {
        gpio_ptr->cfg2.PA17_SELECT = 0;
    }
    if (request->pin == 18) {
        gpio_ptr->cfg2.PA18_SELECT = 0;
    }
    if (request->pin == 19) {
        gpio_ptr->cfg2.PA19_SELECT = 0;
    }
    if (request->pin == 20) {
        gpio_ptr->cfg2.PA20_SELECT = 0;
    }
    if (request->pin == 21) {
        gpio_ptr->cfg2.PA21_SELECT = 0;
    }

    // 之前已经排除pin大于21的情况，所以至此肯定是可以判断和修正出来的

    // 读取gpio_ptr->dat的第几位的值，存入request->value就行
    // 把gpio_ptr->的值右移，舍弃掉低位，这样取的位就在最低位
    // 位与0x1，把高位信息舍去，这样剩下的就只是这个针脚的信息了

    request->value = (*(uint32_t*)(&(gpio_ptr->dat)) >> (request->pin)) & 0x1;

    return 0;
}

// GPIO写出
int32_t gpio_write(char *args, uint32_t args_size) {
    if (args == NULL || args_size != sizeof(struct gpio_request)) {
        return -1;
    }

    struct gpio_request *request = (struct gpio_request *)args;

    // 因为结构都一样，可以直接调整为统一的一个结构体指针来编码
    // 这里选择A组的结构体

    struct gpio_group_A * gpio_ptr = NULL; // 操作的GPIO的指针

    if (request->group == 'A') {
        if (request->pin > 21) {
            // A组只有0到21号针脚
            return -1;
        }
        gpio_ptr = (struct gpio_group_A *)GPIO_A_BASE;
    }
    if (request->group == 'C') {
        if (request->pin > 16) {
            // C组只有0到16号针脚
            return -1;
        }
        gpio_ptr = (struct gpio_group_A *)GPIO_C_BASE;
    }
    if (request->group == 'D') {
        if (request->pin > 17) {
            // D组只有0到17号针脚
            return -1;
        }
        gpio_ptr = (struct gpio_group_A *)GPIO_D_BASE;
    }
    if (request->group == 'E') {
        if (request->pin > 15) {
            // E组只有0到15号针脚
            return -1;
        }
        gpio_ptr = (struct gpio_group_A *)GPIO_E_BASE;
    }
    if (request->group == 'F') {
        if (request->pin > 6) {
            // F组只有0到6号针脚
            return -1;
        }
        gpio_ptr = (struct gpio_group_A *)GPIO_F_BASE;
    }
    if (request->group == 'G') {
        if (request->pin > 13) {
            // G组只有0到13号针脚
            return -1;
        }
        gpio_ptr = (struct gpio_group_A *)GPIO_G_BASE;
    }
    if (request->group == 'L') {
        if (request->pin > 11) {
            // L组只有0到11号针脚
            return -1;
        }
        gpio_ptr = (struct gpio_group_A *)GPIO_L_BASE;
    }

    if(gpio_ptr == NULL) {
        return -1;  // 输入的GPIO组号非法
    }

    // 检查当前引脚是否为输出模式，否则更改为输出模式

    // 为了方便，懒得判断了，直接统一修正为输出模式

    // 我之所以要这么做也是因为我特别厌恶位运算，没办法，我是真的对位运算很不喜欢，我宁愿这样复制粘贴都不想写位运算


    if (request->pin == 0) {
        gpio_ptr->cfg0.PA0_SELECT = 1;
    }
    if (request->pin == 1) {
        gpio_ptr->cfg0.PA1_SELECT = 1;
    }
    if (request->pin == 2) {
        gpio_ptr->cfg0.PA2_SELECT = 1;
    }
    if (request->pin == 3) {
        gpio_ptr->cfg0.PA3_SELECT = 1;
    }
    if (request->pin == 4) {
        gpio_ptr->cfg0.PA4_SELECT = 1;
    }
    if (request->pin == 5) {
        gpio_ptr->cfg0.PA5_SELECT = 1;
    }
    if (request->pin == 6) {
        gpio_ptr->cfg0.PA6_SELECT = 1;
    }
    if (request->pin == 7) {
        gpio_ptr->cfg0.PA7_SELECT = 1;
    }
    if (request->pin == 8) {
        gpio_ptr->cfg1.PA8_SELECT = 1;
    }
    if (request->pin == 9) {
        gpio_ptr->cfg1.PA9_SELECT = 1;
    }
    if (request->pin == 10) {
        gpio_ptr->cfg1.PA10_SELECT = 1;
    }
    if (request->pin == 11) {
        gpio_ptr->cfg1.PA11_SELECT = 1;
    }
    if (request->pin == 12) {
        gpio_ptr->cfg1.PA12_SELECT = 1;
    }
    if (request->pin == 13) {
        gpio_ptr->cfg1.PA13_SELECT = 1;
    }
    if (request->pin == 14) {
        gpio_ptr->cfg1.PA14_SELECT = 1;
    }
    if (request->pin == 15) {
        gpio_ptr->cfg1.PA15_SELECT = 1;
    }
    if (request->pin == 16) {
        gpio_ptr->cfg2.PA16_SELECT = 1;
    }
    if (request->pin == 17) {
        gpio_ptr->cfg2.PA17_SELECT = 1;
    }
    if (request->pin == 18) {
        gpio_ptr->cfg2.PA18_SELECT = 1;
    }
    if (request->pin == 19) {
        gpio_ptr->cfg2.PA19_SELECT = 1;
    }
    if (request->pin == 20) {
        gpio_ptr->cfg2.PA20_SELECT = 1;
    }
    if (request->pin == 21) {
        gpio_ptr->cfg2.PA21_SELECT = 1;
    }

    // 之前已经排除pin大于21的情况，所以至此肯定是可以判断和修正出来的

    // 设置引脚电平值
    // 如果是设置为高电平，直接把1左移到指定位数，然后位或上去就行，这样其他的信息都能被保护
    // 如果是设置为低电平，直接把1左移到指定位数，按位取反，然后位与上去就行，这样其他的信息都能被保护


    if (request->value) {
        *(uint32_t*)(&(gpio_ptr->dat)) |= (1 << request->pin);
    } else {
        *(uint32_t*)(&(gpio_ptr->dat)) &= ~(1 << request->pin);
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
