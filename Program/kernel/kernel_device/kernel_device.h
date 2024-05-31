//
// Created by huangcheng on 2024/5/27.
//

#ifndef HOS_KERNEL_DEVICE_H
#define HOS_KERNEL_DEVICE_H

#include "../../lib/lib_kernel/lib_kernel.h"

// 统一的驱动接口结构体（这个结构体的实例一律放在驱动段）
struct driver {
    char driver_name[64];                       // 驱动名称
    void (*init)(void);                         // 初始化函数
    void (*exit)(void);                         // 退出函数
    int32_t irq;                                // IRQ 中断号（如果没有中断处理函数填-1）
    void (*irq_interrupt_handler)(void);            // 中断处理函数（可选）
    int32_t (*read)(void* buffer, int size);        // 读操作（可选）
    int32_t (*write)(const void* buffer, int size); // 写操作（可选）
    struct list_node node;                      // 用于串联到驱动链表上的链表结点
};

// 初始化所有设备
void init_all_devices();
// 移除所有设备
void exit_all_devices();
// 添加单个驱动并初始化
void driver_add(struct driver* drv);
// 移除单个驱动并卸载
void driver_remove(const char *driver_name);

// 这个注册宏的作用就是把数据放在驱动段（而且告诉编译器这个是被使用的，不能优化掉），它会定义一个驱动结构体实例
#define REGISTER_DRIVER(drv) \
    __attribute__((section(".drivers"), used)) struct driver drv =



#endif //HOS_KERNEL_DEVICE_H
