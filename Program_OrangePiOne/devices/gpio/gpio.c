//
// Created by huangcheng on 2024/8/22.
//

#include "gpio.h"
#include "gpio_datasheet.h"
#include "../../kernel/kernel_device/kernel_device.h"

// GPIO 组信息描述符
// 这个结构体用于描述某一组 GPIO 的 pin 范围
struct gpio_group_info {
    gpio_group_t group;           // GPIO 组枚举值
    uint32_t pin_start;           // 这一组 GPIO 的起始 pin 编号
    uint32_t pin_end;             // 这一组 GPIO 的结束 pin 编号
};


// GPIO 组对应的 pin 范围信息表
// 这里按 H3 数据手册中实际存在的 pin 数量填写
static const struct gpio_group_info gpio_group_info_table[GPIO_GROUP_COUNT] = {
        { GPIO_GROUP_A, 0, 21 },    // PA0  ~ PA21
        { GPIO_GROUP_C, 0, 18 },    // PC0  ~ PC18
        { GPIO_GROUP_D, 0, 17 },    // PD0  ~ PD17
        { GPIO_GROUP_E, 0, 15 },    // PE0  ~ PE15
        { GPIO_GROUP_F, 0, 6  },    // PF0  ~ PF6
        { GPIO_GROUP_G, 0, 13 },    // PG0  ~ PG13
        { GPIO_GROUP_L, 0, 11 }     // PL0  ~ PL11
};


// 判断 GPIO 组是否有效
static inline uint8_t gpio_group_valid(gpio_group_t group) {
    return (group < GPIO_GROUP_COUNT) ? 1 : 0;
}


// 判断某个 GPIO 的 group 和 pin 是否有效
static inline uint8_t gpio_pin_valid(gpio_group_t group, uint32_t pin) {
    if (gpio_group_valid(group) == 0) {
        return 0;
    }

    return (pin >= gpio_group_info_table[group].pin_start &&
            pin <= gpio_group_info_table[group].pin_end) ? 1 : 0;
}


// 获取某个 GPIO 组允许的最大 pin 编号
static inline int32_t gpio_group_max_pin(gpio_group_t group) {
    if (gpio_group_valid(group) == 0) {
        return -1;
    }

    return (int32_t)gpio_group_info_table[group].pin_end;
}


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

// 写针脚输出电平，成功返回0，失败返回-1
int32_t write_pin(uint32_t group, uint32_t pin, uint32_t value);

int32_t read_pin(uint32_t group, uint32_t pin);                             // 读取引脚输入电平，成功返回0或1，失败返回-1

int32_t configure(void *config);                                            // 进行其他配置，参数解释由具体驱动自行决定

static struct gpio_device_operator gpio_operator = {
        .write_pin = write_pin,
        .read_pin = read_pin,
        .configure = configure,
};

// 注册宏，定义一个驱动结构体，把驱动结构体实例放到驱动段
REGISTER_DRIVER(gpio_driver){
        .driver_name = "gpio",
        .init = gpio_init,
        .exit = gpio_exit,
        .irq_descriptors = NULL,
        .irq_count = 0,
        .device_type = 3,
        .device_operator = &gpio_operator,
};

// 获取 GPIO 组的寄存器基地址
static void *gpio_get_group_ptr(gpio_group_t group) {
    if (group == GPIO_GROUP_A) return (void *)GPIO_A_BASE;
    if (group == GPIO_GROUP_C) return (void *)GPIO_C_BASE;
    if (group == GPIO_GROUP_D) return (void *)GPIO_D_BASE;
    if (group == GPIO_GROUP_E) return (void *)GPIO_E_BASE;
    if (group == GPIO_GROUP_F) return (void *)GPIO_F_BASE;
    if (group == GPIO_GROUP_G) return (void *)GPIO_G_BASE;
    if (group == GPIO_GROUP_L) return (void *)GPIO_L_BASE;
    return NULL;
}

// 设置单个pin的功能
static int32_t gpio_set_function(gpio_group_t group, uint32_t pin, uint32_t function) {
    if (gpio_pin_valid(group, pin) == 0) {
        return -1;
    }

    if (function > 7) {
        return -1;
    }

    if (group == GPIO_GROUP_A) {
        struct gpio_group_A *gpio = (struct gpio_group_A *)GPIO_A_BASE;
        switch (pin) {
            case 0:  gpio->cfg0.PA0_SELECT = function; break;
            case 1:  gpio->cfg0.PA1_SELECT = function; break;
            case 2:  gpio->cfg0.PA2_SELECT = function; break;
            case 3:  gpio->cfg0.PA3_SELECT = function; break;
            case 4:  gpio->cfg0.PA4_SELECT = function; break;
            case 5:  gpio->cfg0.PA5_SELECT = function; break;
            case 6:  gpio->cfg0.PA6_SELECT = function; break;
            case 7:  gpio->cfg0.PA7_SELECT = function; break;
            case 8:  gpio->cfg1.PA8_SELECT = function; break;
            case 9:  gpio->cfg1.PA9_SELECT = function; break;
            case 10: gpio->cfg1.PA10_SELECT = function; break;
            case 11: gpio->cfg1.PA11_SELECT = function; break;
            case 12: gpio->cfg1.PA12_SELECT = function; break;
            case 13: gpio->cfg1.PA13_SELECT = function; break;
            case 14: gpio->cfg1.PA14_SELECT = function; break;
            case 15: gpio->cfg1.PA15_SELECT = function; break;
            case 16: gpio->cfg2.PA16_SELECT = function; break;
            case 17: gpio->cfg2.PA17_SELECT = function; break;
            case 18: gpio->cfg2.PA18_SELECT = function; break;
            case 19: gpio->cfg2.PA19_SELECT = function; break;
            case 20: gpio->cfg2.PA20_SELECT = function; break;
            case 21: gpio->cfg2.PA21_SELECT = function; break;
            default: return -1;
        }
        return 0;
    }

    if (group == GPIO_GROUP_C) {
        struct gpio_group_C *gpio = (struct gpio_group_C *)GPIO_C_BASE;
        switch (pin) {
            case 0:  gpio->cfg0.PC0_SELECT = function; break;
            case 1:  gpio->cfg0.PC1_SELECT = function; break;
            case 2:  gpio->cfg0.PC2_SELECT = function; break;
            case 3:  gpio->cfg0.PC3_SELECT = function; break;
            case 4:  gpio->cfg0.PC4_SELECT = function; break;
            case 5:  gpio->cfg0.PC5_SELECT = function; break;
            case 6:  gpio->cfg0.PC6_SELECT = function; break;
            case 7:  gpio->cfg0.PC7_SELECT = function; break;
            case 8:  gpio->cfg1.PC8_SELECT = function; break;
            case 9:  gpio->cfg1.PC9_SELECT = function; break;
            case 10: gpio->cfg1.PC10_SELECT = function; break;
            case 11: gpio->cfg1.PC11_SELECT = function; break;
            case 12: gpio->cfg1.PC12_SELECT = function; break;
            case 13: gpio->cfg1.PC13_SELECT = function; break;
            case 14: gpio->cfg1.PC14_SELECT = function; break;
            case 15: gpio->cfg1.PC15_SELECT = function; break;
            case 16: gpio->cfg2.PC16_SELECT = function; break;
            default: return -1;
        }
        return 0;
    }

    if (group == GPIO_GROUP_D) {
        struct gpio_group_D *gpio = (struct gpio_group_D *)GPIO_D_BASE;
        switch (pin) {
            case 0:  gpio->cfg0.PD0_SELECT = function; break;
            case 1:  gpio->cfg0.PD1_SELECT = function; break;
            case 2:  gpio->cfg0.PD2_SELECT = function; break;
            case 3:  gpio->cfg0.PD3_SELECT = function; break;
            case 4:  gpio->cfg0.PD4_SELECT = function; break;
            case 5:  gpio->cfg0.PD5_SELECT = function; break;
            case 6:  gpio->cfg0.PD6_SELECT = function; break;
            case 7:  gpio->cfg0.PD7_SELECT = function; break;
            case 8:  gpio->cfg1.PD8_SELECT = function; break;
            case 9:  gpio->cfg1.PD9_SELECT = function; break;
            case 10: gpio->cfg1.PD10_SELECT = function; break;
            case 11: gpio->cfg1.PD11_SELECT = function; break;
            case 12: gpio->cfg1.PD12_SELECT = function; break;
            case 13: gpio->cfg1.PD13_SELECT = function; break;
            case 14: gpio->cfg1.PD14_SELECT = function; break;
            case 15: gpio->cfg1.PD15_SELECT = function; break;
            case 16: gpio->cfg2.PD16_SELECT = function; break;
            case 17: gpio->cfg2.PD17_SELECT = function; break;
            default: return -1;
        }
        return 0;
    }

    if (group == GPIO_GROUP_E) {
        struct gpio_group_E *gpio = (struct gpio_group_E *)GPIO_E_BASE;
        switch (pin) {
            case 0:  gpio->cfg0.PE0_SELECT = function; break;
            case 1:  gpio->cfg0.PE1_SELECT = function; break;
            case 2:  gpio->cfg0.PE2_SELECT = function; break;
            case 3:  gpio->cfg0.PE3_SELECT = function; break;
            case 4:  gpio->cfg0.PE4_SELECT = function; break;
            case 5:  gpio->cfg0.PE5_SELECT = function; break;
            case 6:  gpio->cfg0.PE6_SELECT = function; break;
            case 7:  gpio->cfg0.PE7_SELECT = function; break;
            case 8:  gpio->cfg1.PE8_SELECT = function; break;
            case 9:  gpio->cfg1.PE9_SELECT = function; break;
            case 10: gpio->cfg1.PE10_SELECT = function; break;
            case 11: gpio->cfg1.PE11_SELECT = function; break;
            case 12: gpio->cfg1.PE12_SELECT = function; break;
            case 13: gpio->cfg1.PE13_SELECT = function; break;
            case 14: gpio->cfg1.PE14_SELECT = function; break;
            case 15: gpio->cfg1.PE15_SELECT = function; break;
            default: return -1;
        }
        return 0;
    }

    if (group == GPIO_GROUP_F) {
        struct gpio_group_F *gpio = (struct gpio_group_F *)GPIO_F_BASE;
        switch (pin) {
            case 0: gpio->cfg0.PF0_SELECT = function; break;
            case 1: gpio->cfg0.PF1_SELECT = function; break;
            case 2: gpio->cfg0.PF2_SELECT = function; break;
            case 3: gpio->cfg0.PF3_SELECT = function; break;
            case 4: gpio->cfg0.PF4_SELECT = function; break;
            case 5: gpio->cfg0.PF5_SELECT = function; break;
            case 6: gpio->cfg0.PF6_SELECT = function; break;
            default: return -1;
        }
        return 0;
    }

    if (group == GPIO_GROUP_G) {
        struct gpio_group_G *gpio = (struct gpio_group_G *)GPIO_G_BASE;
        switch (pin) {
            case 0:  gpio->cfg0.PG0_SELECT = function; break;
            case 1:  gpio->cfg0.PG1_SELECT = function; break;
            case 2:  gpio->cfg0.PG2_SELECT = function; break;
            case 3:  gpio->cfg0.PG3_SELECT = function; break;
            case 4:  gpio->cfg0.PG4_SELECT = function; break;
            case 5:  gpio->cfg0.PG5_SELECT = function; break;
            case 6:  gpio->cfg0.PG6_SELECT = function; break;
            case 7:  gpio->cfg0.PG7_SELECT = function; break;
            case 8:  gpio->cfg1.PG8_SELECT = function; break;
            case 9:  gpio->cfg1.PG9_SELECT = function; break;
            case 10: gpio->cfg1.PG10_SELECT = function; break;
            case 11: gpio->cfg1.PG11_SELECT = function; break;
            case 12: gpio->cfg1.PG12_SELECT = function; break;
            case 13: gpio->cfg1.PG13_SELECT = function; break;
            default: return -1;
        }
        return 0;
    }

    if (group == GPIO_GROUP_L) {
        struct gpio_group_L *gpio = (struct gpio_group_L *)GPIO_L_BASE;
        switch (pin) {
            case 0:  gpio->cfg0.PL0_SELECT = function; break;
            case 1:  gpio->cfg0.PL1_SELECT = function; break;
            case 2:  gpio->cfg0.PL2_SELECT = function; break;
            case 3:  gpio->cfg0.PL3_SELECT = function; break;
            case 4:  gpio->cfg0.PL4_SELECT = function; break;
            case 5:  gpio->cfg0.PL5_SELECT = function; break;
            case 6:  gpio->cfg0.PL6_SELECT = function; break;
            case 7:  gpio->cfg0.PL7_SELECT = function; break;
            case 8:  gpio->cfg1.PL8_SELECT = function; break;
            case 9:  gpio->cfg1.PL9_SELECT = function; break;
            case 10: gpio->cfg1.PL10_SELECT = function; break;
            case 11: gpio->cfg1.PL11_SELECT = function; break;
            default: return -1;
        }
        return 0;
    }

    return -1;
}

// 设置单个pin的上下拉电阻
static int32_t gpio_set_pull(gpio_group_t group, uint32_t pin, uint32_t pull) {
    if (gpio_pin_valid(group, pin) == 0) {
        return -1;
    }

    if (pull > 2) {
        return -1;
    }

    if (group == GPIO_GROUP_A) {
        struct gpio_group_A *gpio = (struct gpio_group_A *)GPIO_A_BASE;
        switch (pin) {
            case 0:  gpio->pull0.PA0_PULL = pull; break;
            case 1:  gpio->pull0.PA1_PULL = pull; break;
            case 2:  gpio->pull0.PA2_PULL = pull; break;
            case 3:  gpio->pull0.PA3_PULL = pull; break;
            case 4:  gpio->pull0.PA4_PULL = pull; break;
            case 5:  gpio->pull0.PA5_PULL = pull; break;
            case 6:  gpio->pull0.PA6_PULL = pull; break;
            case 7:  gpio->pull0.PA7_PULL = pull; break;
            case 8:  gpio->pull0.PA8_PULL = pull; break;
            case 9:  gpio->pull0.PA9_PULL = pull; break;
            case 10: gpio->pull0.PA10_PULL = pull; break;
            case 11: gpio->pull0.PA11_PULL = pull; break;
            case 12: gpio->pull0.PA12_PULL = pull; break;
            case 13: gpio->pull0.PA13_PULL = pull; break;
            case 14: gpio->pull0.PA14_PULL = pull; break;
            case 15: gpio->pull0.PA15_PULL = pull; break;
            case 16: gpio->pull1.PA16_PULL = pull; break;
            case 17: gpio->pull1.PA17_PULL = pull; break;
            case 18: gpio->pull1.PA18_PULL = pull; break;
            case 19: gpio->pull1.PA19_PULL = pull; break;
            case 20: gpio->pull1.PA20_PULL = pull; break;
            case 21: gpio->pull1.PA21_PULL = pull; break;
            default: return -1;
        }
        return 0;
    }

    if (group == GPIO_GROUP_C) {
        struct gpio_group_C *gpio = (struct gpio_group_C *)GPIO_C_BASE;
        switch (pin) {
            case 0:  gpio->pull0.PC0_PULL = pull; break;
            case 1:  gpio->pull0.PC1_PULL = pull; break;
            case 2:  gpio->pull0.PC2_PULL = pull; break;
            case 3:  gpio->pull0.PC3_PULL = pull; break;
            case 4:  gpio->pull0.PC4_PULL = pull; break;
            case 5:  gpio->pull0.PC5_PULL = pull; break;
            case 6:  gpio->pull0.PC6_PULL = pull; break;
            case 7:  gpio->pull0.PC7_PULL = pull; break;
            case 8:  gpio->pull0.PC8_PULL = pull; break;
            case 9:  gpio->pull0.PC9_PULL = pull; break;
            case 10: gpio->pull0.PC10_PULL = pull; break;
            case 11: gpio->pull0.PC11_PULL = pull; break;
            case 12: gpio->pull0.PC12_PULL = pull; break;
            case 13: gpio->pull0.PC13_PULL = pull; break;
            case 14: gpio->pull0.PC14_PULL = pull; break;
            case 15: gpio->pull0.PC15_PULL = pull; break;
            case 16: gpio->pull1.PC16_PULL = pull; break;
            case 17: gpio->pull1.PC17_PULL = pull; break;
            case 18: gpio->pull1.PC18_PULL = pull; break;
            default: return -1;
        }
        return 0;
    }

    if (group == GPIO_GROUP_D) {
        struct gpio_group_D *gpio = (struct gpio_group_D *)GPIO_D_BASE;
        switch (pin) {
            case 0:  gpio->pull0.PD0_PULL = pull; break;
            case 1:  gpio->pull0.PD1_PULL = pull; break;
            case 2:  gpio->pull0.PD2_PULL = pull; break;
            case 3:  gpio->pull0.PD3_PULL = pull; break;
            case 4:  gpio->pull0.PD4_PULL = pull; break;
            case 5:  gpio->pull0.PD5_PULL = pull; break;
            case 6:  gpio->pull0.PD6_PULL = pull; break;
            case 7:  gpio->pull0.PD7_PULL = pull; break;
            case 8:  gpio->pull0.PD8_PULL = pull; break;
            case 9:  gpio->pull0.PD9_PULL = pull; break;
            case 10: gpio->pull0.PD10_PULL = pull; break;
            case 11: gpio->pull0.PD11_PULL = pull; break;
            case 12: gpio->pull0.PD12_PULL = pull; break;
            case 13: gpio->pull0.PD13_PULL = pull; break;
            case 14: gpio->pull0.PD14_PULL = pull; break;
            case 15: gpio->pull0.PD15_PULL = pull; break;
            case 16: gpio->pull1.PD16_PULL = pull; break;
            case 17: gpio->pull1.PD17_PULL = pull; break;
            default: return -1;
        }
        return 0;
    }

    if (group == GPIO_GROUP_E) {
        struct gpio_group_E *gpio = (struct gpio_group_E *)GPIO_E_BASE;
        switch (pin) {
            case 0:  gpio->pull0.PE0_PULL = pull; break;
            case 1:  gpio->pull0.PE1_PULL = pull; break;
            case 2:  gpio->pull0.PE2_PULL = pull; break;
            case 3:  gpio->pull0.PE3_PULL = pull; break;
            case 4:  gpio->pull0.PE4_PULL = pull; break;
            case 5:  gpio->pull0.PE5_PULL = pull; break;
            case 6:  gpio->pull0.PE6_PULL = pull; break;
            case 7:  gpio->pull0.PE7_PULL = pull; break;
            case 8:  gpio->pull0.PE8_PULL = pull; break;
            case 9:  gpio->pull0.PE9_PULL = pull; break;
            case 10: gpio->pull0.PE10_PULL = pull; break;
            case 11: gpio->pull0.PE11_PULL = pull; break;
            case 12: gpio->pull0.PE12_PULL = pull; break;
            case 13: gpio->pull0.PE13_PULL = pull; break;
            case 14: gpio->pull0.PE14_PULL = pull; break;
            case 15: gpio->pull0.PE15_PULL = pull; break;
            default: return -1;
        }
        return 0;
    }

    if (group == GPIO_GROUP_F) {
        struct gpio_group_F *gpio = (struct gpio_group_F *)GPIO_F_BASE;
        switch (pin) {
            case 0: gpio->pull0.PF0_PULL = pull; break;
            case 1: gpio->pull0.PF1_PULL = pull; break;
            case 2: gpio->pull0.PF2_PULL = pull; break;
            case 3: gpio->pull0.PF3_PULL = pull; break;
            case 4: gpio->pull0.PF4_PULL = pull; break;
            case 5: gpio->pull0.PF5_PULL = pull; break;
            case 6: gpio->pull0.PF6_PULL = pull; break;
            default: return -1;
        }
        return 0;
    }

    if (group == GPIO_GROUP_G) {
        struct gpio_group_G *gpio = (struct gpio_group_G *)GPIO_G_BASE;
        switch (pin) {
            case 0:  gpio->pull0.PG0_PULL = pull; break;
            case 1:  gpio->pull0.PG1_PULL = pull; break;
            case 2:  gpio->pull0.PG2_PULL = pull; break;
            case 3:  gpio->pull0.PG3_PULL = pull; break;
            case 4:  gpio->pull0.PG4_PULL = pull; break;
            case 5:  gpio->pull0.PG5_PULL = pull; break;
            case 6:  gpio->pull0.PG6_PULL = pull; break;
            case 7:  gpio->pull0.PG7_PULL = pull; break;
            case 8:  gpio->pull0.PG8_PULL = pull; break;
            case 9:  gpio->pull0.PG9_PULL = pull; break;
            case 10: gpio->pull0.PG10_PULL = pull; break;
            case 11: gpio->pull0.PG11_PULL = pull; break;
            case 12: gpio->pull0.PG12_PULL = pull; break;
            case 13: gpio->pull0.PG13_PULL = pull; break;
            default: return -1;
        }
        return 0;
    }

    if (group == GPIO_GROUP_L) {
        struct gpio_group_L *gpio = (struct gpio_group_L *)GPIO_L_BASE;
        switch (pin) {
            case 0:  gpio->pull0.PL0_PULL = pull; break;
            case 1:  gpio->pull0.PL1_PULL = pull; break;
            case 2:  gpio->pull0.PL2_PULL = pull; break;
            case 3:  gpio->pull0.PL3_PULL = pull; break;
            case 4:  gpio->pull0.PL4_PULL = pull; break;
            case 5:  gpio->pull0.PL5_PULL = pull; break;
            case 6:  gpio->pull0.PL6_PULL = pull; break;
            case 7:  gpio->pull0.PL7_PULL = pull; break;
            case 8:  gpio->pull0.PL8_PULL = pull; break;
            case 9:  gpio->pull0.PL9_PULL = pull; break;
            case 10: gpio->pull0.PL10_PULL = pull; break;
            case 11: gpio->pull0.PL11_PULL = pull; break;
            default: return -1;
        }
        return 0;
    }

    return -1;
}

// 设置单个pin的驱动等级
static int32_t gpio_set_drive_level(gpio_group_t group, uint32_t pin, uint32_t drive_level) {
    if (gpio_pin_valid(group, pin) == 0) {
        return -1;
    }

    if (drive_level > 3) {
        return -1;
    }

    if (group == GPIO_GROUP_A) {
        struct gpio_group_A *gpio = (struct gpio_group_A *)GPIO_A_BASE;
        switch (pin) {
            case 0:  gpio->drv0.PA0_DRV = drive_level; break;
            case 1:  gpio->drv0.PA1_DRV = drive_level; break;
            case 2:  gpio->drv0.PA2_DRV = drive_level; break;
            case 3:  gpio->drv0.PA3_DRV = drive_level; break;
            case 4:  gpio->drv0.PA4_DRV = drive_level; break;
            case 5:  gpio->drv0.PA5_DRV = drive_level; break;
            case 6:  gpio->drv0.PA6_DRV = drive_level; break;
            case 7:  gpio->drv0.PA7_DRV = drive_level; break;
            case 8:  gpio->drv0.PA8_DRV = drive_level; break;
            case 9:  gpio->drv0.PA9_DRV = drive_level; break;
            case 10: gpio->drv0.PA10_DRV = drive_level; break;
            case 11: gpio->drv0.PA11_DRV = drive_level; break;
            case 12: gpio->drv0.PA12_DRV = drive_level; break;
            case 13: gpio->drv0.PA13_DRV = drive_level; break;
            case 14: gpio->drv0.PA14_DRV = drive_level; break;
            case 15: gpio->drv0.PA15_DRV = drive_level; break;
            case 16: gpio->drv1.PA16_DRV = drive_level; break;
            case 17: gpio->drv1.PA17_DRV = drive_level; break;
            case 18: gpio->drv1.PA18_DRV = drive_level; break;
            case 19: gpio->drv1.PA19_DRV = drive_level; break;
            case 20: gpio->drv1.PA20_DRV = drive_level; break;
            case 21: gpio->drv1.PA21_DRV = drive_level; break;
            default: return -1;
        }
        return 0;
    }

    if (group == GPIO_GROUP_C) {
        struct gpio_group_C *gpio = (struct gpio_group_C *)GPIO_C_BASE;
        switch (pin) {
            case 0:  gpio->drv0.PC0_DRV = drive_level; break;
            case 1:  gpio->drv0.PC1_DRV = drive_level; break;
            case 2:  gpio->drv0.PC2_DRV = drive_level; break;
            case 3:  gpio->drv0.PC3_DRV = drive_level; break;
            case 4:  gpio->drv0.PC4_DRV = drive_level; break;
            case 5:  gpio->drv0.PC5_DRV = drive_level; break;
            case 6:  gpio->drv0.PC6_DRV = drive_level; break;
            case 7:  gpio->drv0.PC7_DRV = drive_level; break;
            case 8:  gpio->drv0.PC8_DRV = drive_level; break;
            case 9:  gpio->drv0.PC9_DRV = drive_level; break;
            case 10: gpio->drv0.PC10_DRV = drive_level; break;
            case 11: gpio->drv0.PC11_DRV = drive_level; break;
            case 12: gpio->drv0.PC12_DRV = drive_level; break;
            case 13: gpio->drv0.PC13_DRV = drive_level; break;
            case 14: gpio->drv0.PC14_DRV = drive_level; break;
            case 15: gpio->drv0.PC15_DRV = drive_level; break;
            case 16: gpio->drv1.PC16_DRV = drive_level; break;
            case 17: gpio->drv1.PC17_DRV = drive_level; break;
            case 18: gpio->drv1.PC18_DRV = drive_level; break;
            default: return -1;
        }
        return 0;
    }

    if (group == GPIO_GROUP_D) {
        struct gpio_group_D *gpio = (struct gpio_group_D *)GPIO_D_BASE;
        switch (pin) {
            case 0:  gpio->drv0.PD0_DRV = drive_level; break;
            case 1:  gpio->drv0.PD1_DRV = drive_level; break;
            case 2:  gpio->drv0.PD2_DRV = drive_level; break;
            case 3:  gpio->drv0.PD3_DRV = drive_level; break;
            case 4:  gpio->drv0.PD4_DRV = drive_level; break;
            case 5:  gpio->drv0.PD5_DRV = drive_level; break;
            case 6:  gpio->drv0.PD6_DRV = drive_level; break;
            case 7:  gpio->drv0.PD7_DRV = drive_level; break;
            case 8:  gpio->drv0.PD8_DRV = drive_level; break;
            case 9:  gpio->drv0.PD9_DRV = drive_level; break;
            case 10: gpio->drv0.PD10_DRV = drive_level; break;
            case 11: gpio->drv0.PD11_DRV = drive_level; break;
            case 12: gpio->drv0.PD12_DRV = drive_level; break;
            case 13: gpio->drv0.PD13_DRV = drive_level; break;
            case 14: gpio->drv0.PD14_DRV = drive_level; break;
            case 15: gpio->drv0.PD15_DRV = drive_level; break;
            case 16: gpio->drv1.PD16_DRV = drive_level; break;
            case 17: gpio->drv1.PD17_DRV = drive_level; break;
            default: return -1;
        }
        return 0;
    }

    if (group == GPIO_GROUP_E) {
        struct gpio_group_E *gpio = (struct gpio_group_E *)GPIO_E_BASE;
        switch (pin) {
            case 0:  gpio->drv0.PE0_DRV = drive_level; break;
            case 1:  gpio->drv0.PE1_DRV = drive_level; break;
            case 2:  gpio->drv0.PE2_DRV = drive_level; break;
            case 3:  gpio->drv0.PE3_DRV = drive_level; break;
            case 4:  gpio->drv0.PE4_DRV = drive_level; break;
            case 5:  gpio->drv0.PE5_DRV = drive_level; break;
            case 6:  gpio->drv0.PE6_DRV = drive_level; break;
            case 7:  gpio->drv0.PE7_DRV = drive_level; break;
            case 8:  gpio->drv0.PE8_DRV = drive_level; break;
            case 9:  gpio->drv0.PE9_DRV = drive_level; break;
            case 10: gpio->drv0.PE10_DRV = drive_level; break;
            case 11: gpio->drv0.PE11_DRV = drive_level; break;
            case 12: gpio->drv0.PE12_DRV = drive_level; break;
            case 13: gpio->drv0.PE13_DRV = drive_level; break;
            case 14: gpio->drv0.PE14_DRV = drive_level; break;
            case 15: gpio->drv0.PE15_DRV = drive_level; break;
            default: return -1;
        }
        return 0;
    }

    if (group == GPIO_GROUP_F) {
        struct gpio_group_F *gpio = (struct gpio_group_F *)GPIO_F_BASE;
        switch (pin) {
            case 0: gpio->drv0.PF0_DRV = drive_level; break;
            case 1: gpio->drv0.PF1_DRV = drive_level; break;
            case 2: gpio->drv0.PF2_DRV = drive_level; break;
            case 3: gpio->drv0.PF3_DRV = drive_level; break;
            case 4: gpio->drv0.PF4_DRV = drive_level; break;
            case 5: gpio->drv0.PF5_DRV = drive_level; break;
            case 6: gpio->drv0.PF6_DRV = drive_level; break;
            default: return -1;
        }
        return 0;
    }

    if (group == GPIO_GROUP_G) {
        struct gpio_group_G *gpio = (struct gpio_group_G *)GPIO_G_BASE;
        switch (pin) {
            case 0:  gpio->drv0.PG0_DRV = drive_level; break;
            case 1:  gpio->drv0.PG1_DRV = drive_level; break;
            case 2:  gpio->drv0.PG2_DRV = drive_level; break;
            case 3:  gpio->drv0.PG3_DRV = drive_level; break;
            case 4:  gpio->drv0.PG4_DRV = drive_level; break;
            case 5:  gpio->drv0.PG5_DRV = drive_level; break;
            case 6:  gpio->drv0.PG6_DRV = drive_level; break;
            case 7:  gpio->drv0.PG7_DRV = drive_level; break;
            case 8:  gpio->drv0.PG8_DRV = drive_level; break;
            case 9:  gpio->drv0.PG9_DRV = drive_level; break;
            case 10: gpio->drv0.PG10_DRV = drive_level; break;
            case 11: gpio->drv0.PG11_DRV = drive_level; break;
            case 12: gpio->drv0.PG12_DRV = drive_level; break;
            case 13: gpio->drv0.PG13_DRV = drive_level; break;
            default: return -1;
        }
        return 0;
    }

    if (group == GPIO_GROUP_L) {
        struct gpio_group_L *gpio = (struct gpio_group_L *)GPIO_L_BASE;
        switch (pin) {
            case 0:  gpio->drv0.PL0_DRV = drive_level; break;
            case 1:  gpio->drv0.PL1_DRV = drive_level; break;
            case 2:  gpio->drv0.PL2_DRV = drive_level; break;
            case 3:  gpio->drv0.PL3_DRV = drive_level; break;
            case 4:  gpio->drv0.PL4_DRV = drive_level; break;
            case 5:  gpio->drv0.PL5_DRV = drive_level; break;
            case 6:  gpio->drv0.PL6_DRV = drive_level; break;
            case 7:  gpio->drv0.PL7_DRV = drive_level; break;
            case 8:  gpio->drv0.PL8_DRV = drive_level; break;
            case 9:  gpio->drv0.PL9_DRV = drive_level; break;
            case 10: gpio->drv0.PL10_DRV = drive_level; break;
            case 11: gpio->drv0.PL11_DRV = drive_level; break;
            default: return -1;
        }
        return 0;
    }

    return -1;
}

// 写针脚输出电平，成功返回0，失败返回-1
int32_t write_pin(uint32_t group, uint32_t pin, uint32_t value) {
    // 第一步：检查 group 和 pin 是否有效
    if (gpio_pin_valid((gpio_group_t)group, pin) == 0) {
        return -1;
    }

    // 第二步：GPIO 电平值只能是 0 或 1
    if (value != 0 && value != 1) {
        return -1;
    }

    // 第三步：写之前，先强制把这个 pin 修正成输出模式
    // 这样调用者就不用额外先 configure 成输出了
    if (gpio_set_function((gpio_group_t)group, pin, GPIO_PIN_FUNC_OUTPUT) != 0) {
        return -1;
    }

    // 第四步：找到对应 GPIO 组的数据寄存器地址
    //
    // 注意这里不是取整个 group 的首地址，而是取 group 结构体里面 dat 这个成员的地址。
    // 例如 A 组：
    //   struct gpio_group_A {
    //       ...
    //       PA_DATA_REG_t dat;
    //       ...
    //   };
    //
    // 我们把 &gpio->dat 转成 uint32_t *，这样就能把这个数据寄存器当作一个 32 位整数来处理。
    volatile uint32_t *dat_reg = NULL;

    if (group == GPIO_GROUP_A) {
        struct gpio_group_A *gpio = (struct gpio_group_A *)GPIO_A_BASE;
        dat_reg = (volatile uint32_t *)&(gpio->dat);
    } else if (group == GPIO_GROUP_C) {
        struct gpio_group_C *gpio = (struct gpio_group_C *)GPIO_C_BASE;
        dat_reg = (volatile uint32_t *)&(gpio->dat);
    } else if (group == GPIO_GROUP_D) {
        struct gpio_group_D *gpio = (struct gpio_group_D *)GPIO_D_BASE;
        dat_reg = (volatile uint32_t *)&(gpio->dat);
    } else if (group == GPIO_GROUP_E) {
        struct gpio_group_E *gpio = (struct gpio_group_E *)GPIO_E_BASE;
        dat_reg = (volatile uint32_t *)&(gpio->dat);
    } else if (group == GPIO_GROUP_F) {
        struct gpio_group_F *gpio = (struct gpio_group_F *)GPIO_F_BASE;
        dat_reg = (volatile uint32_t *)&(gpio->dat);
    } else if (group == GPIO_GROUP_G) {
        struct gpio_group_G *gpio = (struct gpio_group_G *)GPIO_G_BASE;
        dat_reg = (volatile uint32_t *)&(gpio->dat);
    } else if (group == GPIO_GROUP_L) {
        struct gpio_group_L *gpio = (struct gpio_group_L *)GPIO_L_BASE;
        dat_reg = (volatile uint32_t *)&(gpio->dat);
    } else {
        return -1;
    }

    // 第五步：构造“只作用于某一位”的掩码 mask
    //
    // 例如：
    //   pin = 0  => mask = 00000001b
    //   pin = 1  => mask = 00000010b
    //   pin = 2  => mask = 00000100b
    //   pin = 5  => mask = 00100000b
    //
    // 写法：
    //   1U << pin
    //
    // 解释：
    //   1U 的二进制一开始是：
    //       00000000 00000000 00000000 00000001
    //
    //   如果 pin = 3，左移 3 位后变成：
    //       00000000 00000000 00000000 00001000
    //
    //   这就表示“第 3 位的位置上是 1，其余位都是 0”
    //
    // 这个东西就叫掩码。它只选中目标 pin，对其他 pin 没影响。
    uint32_t mask = (1U << pin);

    // 第六步：根据 value 决定是置 1 还是清 0
    if (value == 1) {
        // -----------------------------
        // 要把某一位写成 1，用按位或：|
        // -----------------------------
        //
        // 公式：
        //   新值 = 旧值 | mask
        //
        // 因为按位或的规则是：
        //   0 | 0 = 0
        //   0 | 1 = 1
        //   1 | 0 = 1
        //   1 | 1 = 1
        //
        // 也就是说，只要 mask 的目标位是 1，
        // 那么旧值这一位不管原来是 0 还是 1，结果都会变成 1。
        //
        // 举例：
        //   假设当前寄存器值是：
        //       00010010
        //   现在 pin = 2
        //   那么 mask = 00000100
        //
        //   做按位或：
        //       00010010
        //     | 00000100
        //     = 00010110
        //
        // 可以看到第 2 位被写成 1 了，其他位保持不变。
        *dat_reg = (*dat_reg) | mask;
    } else {
        // -----------------------------------------
        // 要把某一位写成 0，先把 mask 取反，再按位与：&
        // -----------------------------------------
        //
        // 先看 mask：
        //   假设 pin = 2
        //   mask = 00000100
        //
        // 把它按位取反 ~mask：
        //   ~mask = 11111011
        //
        // 然后做：
        //   新值 = 旧值 & (~mask)
        //
        // 因为按位与的规则是：
        //   0 & 0 = 0
        //   0 & 1 = 0
        //   1 & 0 = 0
        //   1 & 1 = 1
        //
        // 也就是说：
        //   - 目标位由于 ~mask 那一位是 0，所以结果一定变成 0
        //   - 其他位由于 ~mask 那些位都是 1，所以原值会被保留下来
        //
        // 举例：
        //   假设当前寄存器值是：
        //       00010110
        //   pin = 2
        //   mask =    00000100
        //   ~mask =   11111011
        //
        //   做按位与：
        //       00010110
        //     & 11111011
        //     = 00010010
        //
        // 可以看到第 2 位被清成 0，其他位不变。
        *dat_reg = (*dat_reg) & (~mask);
    }

    return 0;
}

// 读取针脚输入电平，成功返回0或1，失败返回-1
int32_t read_pin(uint32_t group, uint32_t pin) {
    // 第一步：检查 group 和 pin 是否有效
    if (gpio_pin_valid((gpio_group_t)group, pin) == 0) {
        return -1;
    }

    // 第二步：读取之前，先统一修正成输入模式
    if (gpio_set_function((gpio_group_t)group, pin, GPIO_PIN_FUNC_INPUT) != 0) {
        return -1;
    }

    // 第三步：拿到对应组的数据寄存器地址
    volatile uint32_t *dat_reg = NULL;

    if (group == GPIO_GROUP_A) {
        struct gpio_group_A *gpio = (struct gpio_group_A *)GPIO_A_BASE;
        dat_reg = (volatile uint32_t *)&(gpio->dat);
    } else if (group == GPIO_GROUP_C) {
        struct gpio_group_C *gpio = (struct gpio_group_C *)GPIO_C_BASE;
        dat_reg = (volatile uint32_t *)&(gpio->dat);
    } else if (group == GPIO_GROUP_D) {
        struct gpio_group_D *gpio = (struct gpio_group_D *)GPIO_D_BASE;
        dat_reg = (volatile uint32_t *)&(gpio->dat);
    } else if (group == GPIO_GROUP_E) {
        struct gpio_group_E *gpio = (struct gpio_group_E *)GPIO_E_BASE;
        dat_reg = (volatile uint32_t *)&(gpio->dat);
    } else if (group == GPIO_GROUP_F) {
        struct gpio_group_F *gpio = (struct gpio_group_F *)GPIO_F_BASE;
        dat_reg = (volatile uint32_t *)&(gpio->dat);
    } else if (group == GPIO_GROUP_G) {
        struct gpio_group_G *gpio = (struct gpio_group_G *)GPIO_G_BASE;
        dat_reg = (volatile uint32_t *)&(gpio->dat);
    } else if (group == GPIO_GROUP_L) {
        struct gpio_group_L *gpio = (struct gpio_group_L *)GPIO_L_BASE;
        dat_reg = (volatile uint32_t *)&(gpio->dat);
    } else {
        return -1;
    }

    // 第四步：把目标 pin 那一位“移动到最低位”
    //
    // 这里用到右移：
    //   (*dat_reg) >> pin
    //
    // 假设当前寄存器值是：
    //   00101100
    //
    // 如果 pin = 3
    // 那么我们关心的是“第 3 位”
    //
    // 右移 3 位后：
    //   00101100 >> 3 = 00000101
    //
    // 原本第 3 位，就被移动到了最低位 bit0 的位置。
    //
    // 但这时候高位还残留着别的信息，我们还要再做一步筛选。
    uint32_t shifted_value = (*dat_reg) >> pin;

    // 第五步：只取最低位
    //
    // 方法是跟 0x1 做按位与：
    //   result = shifted_value & 0x1
    //
    // 0x1 的二进制是：
    //   00000001
    //
    // 按位与之后：
    //   - 只有最低位会被保留
    //   - 其他位全部清零
    //
    // 举例：
    //   shifted_value = 00000101
    //   0x1          = 00000001
    //
    //   做按位与：
    //      00000101
    //    & 00000001
    //    = 00000001
    //
    // 结果就是 1，表示这个 pin 当前是高电平。
    //
    // 如果 shifted_value = 00000100
    // 那么：
    //      00000100
    //    & 00000001
    //    = 00000000
    //
    // 结果就是 0，表示这个 pin 当前是低电平。
    uint32_t result = shifted_value & 0x1U;

    return (int32_t)result;
}

// 配置单个 GPIO pin
int32_t configure(void *config) {
    struct gpio_config *cfg = (struct gpio_config *)config;

    // 第一步：空指针检查
    if (cfg == NULL) {
        return -1;
    }

    // 第二步：检查 pin 是否有效
    if (gpio_pin_valid(cfg->group, cfg->pin) == 0) {
        return -1;
    }

    // 第三步：检查功能模式范围
    if (cfg->function > GPIO_PIN_FUNC_DISABLE) {
        return -1;
    }

    // 第四步：检查上下拉范围
    if (cfg->pull > GPIO_PULL_DOWN) {
        return -1;
    }

    // 第五步：检查驱动能力范围
    if (cfg->drive_level > GPIO_DRIVE_LEVEL3) {
        return -1;
    }

    // 第六步：检查初始输出值范围
    // -1 表示“不处理初始输出值”
    // 0 表示“如果是输出，就初始化为低电平”
    // 1 表示“如果是输出，就初始化为高电平”
    if (cfg->init_value < -1 || cfg->init_value > 1) {
        return -1;
    }

    // 第七步：配置功能模式
    if (gpio_set_function(cfg->group, cfg->pin, cfg->function) != 0) {
        return -1;
    }

    // 第八步：配置上下拉
    if (gpio_set_pull(cfg->group, cfg->pin, cfg->pull) != 0) {
        return -1;
    }

    // 第九步：配置驱动能力
    if (gpio_set_drive_level(cfg->group, cfg->pin, cfg->drive_level) != 0) {
        return -1;
    }

    // 第十步：如果调用者要求设置初始输出值，并且当前模式是输出模式，就补一次 write_pin
    if ((cfg->init_value == 0 || cfg->init_value == 1) &&
        (cfg->function == GPIO_PIN_FUNC_OUTPUT)) {
        if (write_pin(cfg->group, cfg->pin, (uint32_t)cfg->init_value) != 0) {
            return -1;
        }
    }

    return 0;
}
