//
// Created by huangcheng on 2026/4/10.
//

#ifndef HOS_KERNEL_DEVICE_COMMON_H
#define HOS_KERNEL_DEVICE_COMMON_H

#include "../../lib/lib_kernel/lib_kernel.h"

// 这个头文件放的是驱动管理框架的公用数据类型

// 目前分类为字符设备和块设备两种，提供最基础的操作集


// 字符设备操作集
// 适合串口、键盘、终端之类按字节流或字符流工作的设备
// 如果某个字符设备不支持某个操作，对应函数指针填NULL即可
struct char_device_operator {

    int32_t (*read)(char *data, uint32_t count);                // 读字符设备，成功返回读出字节数，失败返回-1
    int32_t (*write)(const char *data, uint32_t count);         // 写字符设备，成功返回写入字节数，失败返回-1

    int32_t (*configure)(void *config);                         // 配置字符设备，参数解释由具体驱动自行决定
};


// 块设备操作集
// 适合硬盘、SD卡这类按块访问的设备
// 一般以逻辑块号（LBA）为单位进行读写
struct block_device_operator {
    uint32_t block_size;                                        // 设备块大小（单位：字节）

    int32_t (*read_block)(uint32_t lba, void *buffer, uint32_t block_count);          // 从指定逻辑块号开始读若干块
    int32_t (*write_block)(uint32_t lba, const void *buffer, uint32_t block_count);   // 从指定逻辑块号开始写若干块

    int32_t (*sync)(void);                                      // 将块设备中的缓存数据同步到实际设备，成功返回0，失败返回-1
    int32_t (*configure)(void *config);                         // 配置块设备，参数解释由具体驱动自行决定
};

// GPIO设备操作集
// 适合对GPIO引脚进行方向配置、电平读写、中断配置等操作
struct gpio_device_operator {
    int32_t (*write_pin)(uint32_t group, uint32_t pin, uint32_t value);            // 设置引脚输出电平，成功返回0，失败返回-1
    int32_t (*read_pin)(uint32_t group, uint32_t pin);                             // 读取引脚输入电平，成功返回0或1，失败返回-1
    int32_t (*configure)(void *config);                                            // 进行配置，参数解释由具体驱动自行决定
};

// IRQ 描述符
// 一个驱动可以有多个 IRQ，每个 IRQ 都可以有自己的触发方式和中断处理函数
struct driver_irq_descriptor {
    int32_t irq;                                // IRQ 中断号（无效就填-1）
    uint32_t trigger_mode;                      // 触发方式，电平触发为0（默认为电平触发），边缘触发为1
    void (*irq_interrupt_handler)(void);        // 对应这个 IRQ 的中断处理函数
};

struct driver_descriptor {
    char driver_name[64];                                       // 驱动名称

    void (*init)(void);                                         // 初始化函数
    void (*exit)(void);                                         // 退出函数

    // IRQ 描述符数组及其数量
    // 如果驱动没有 IRQ，就把 irq_descriptors 填 NULL，irq_count 填 0
    const struct driver_irq_descriptor *irq_descriptors;        // 指向 IRQ 描述符数组的指针
    uint32_t irq_count;                                         // IRQ 描述符数量

    uint32_t device_type;                                       // 驱动类型，目前之确定4种设备类型，0为不需要操作的设备，1为字符设备，2为块设备，3为GPIO设备
    void *device_operator;                                      // 指向具体设备操作集的指针，比如字符设备、块设备等

    struct list_node node;                                      // 用于串联到驱动链表上的链表结点
};

// 这个注册宏的作用就是把数据放在驱动段（而且告诉编译器这个是被使用的，不能优化掉），它会定义一个驱动结构体实例（增加4字节对齐）
#define REGISTER_DRIVER(drv) \
    __attribute__((section(".drivers"), used, aligned(4))) struct driver_descriptor drv =


#endif //HOS_KERNEL_DEVICE_COMMON_H
