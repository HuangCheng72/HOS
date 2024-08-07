//
// Created by huangcheng on 2024/5/27.
//

#ifndef HOS_KERNEL_DEVICE_H
#define HOS_KERNEL_DEVICE_H

#include "../../lib/lib_kernel/lib_kernel.h"
#include "../kernel_buffer/kernel_buffer.h"

// 统一的驱动接口结构体（这个结构体的实例一律放在驱动段）
struct driver {
    char driver_name[64];                       // 驱动名称
    void (*init)(void);                         // 初始化函数
    void (*exit)(void);                         // 退出函数

    // 驱动提供的读写函数（如果没有就读写缓冲区）

    int32_t (*read)(char *args, uint32_t args_size);    // 读设备函数（具体参数写在args里面）
    int32_t (*write)(char *args, uint32_t args_size);   // 写设备函数（具体参数写在args里面）


    int32_t irq;                                // IRQ 中断号（如果没有中断处理函数填-1）
    uint32_t trigger_mode;                      // 触发方式，电平触发为0（默认为电平触发），边缘触发为1
    void (*irq_interrupt_handler)(void);        // 中断处理函数（可选）

    // 因为注册宏里面改了四字节对齐，这里用uint_8_t也是四字节，干脆用uint_32_t填满

    uint32_t need_data_buffer;                  // 需要分配数据缓冲区就填1，不然填0
    uint32_t need_command_buffer;               // 需要分配命令缓冲区就填1，不然填0

    struct kernel_buffer *data_buffer;          // 指向数据缓冲区的指针
    struct kernel_buffer *command_buffer;       // 指向命令缓冲区的指针

    struct list_node node;                      // 用于串联到驱动链表上的链表结点

    void *driver_task_function;                 // 用于创建内核任务的函数入口
    // 这个内核任务负责具体的对设备读写，与缓冲区进行交互
    // 依赖于设备驱动的实现，没有也无所谓

    void *driver_task_function_args;            // 理论上应该不用传参，实际上不知道，多冗余一些好
};

// 初始化所有设备
void init_all_devices();
// 移除所有设备
void exit_all_devices();
// 添加单个驱动并初始化
void driver_add(struct driver* drv);
// 移除单个驱动并卸载
void driver_remove(const char *driver_name);
// 获取设备驱动
struct driver* get_driver(const char *driver_name);
// 对设备（驱动）读，如果设备驱动提供了read就转发（返回结果设备驱动read的结果），不然读其数据缓冲区（成功返回读出数量（以字节计算），不成功返回-1）
int32_t device_read(struct driver *drv, char *data, uint32_t count);
// 对设备（驱动）写，如果设备驱动提供了write就转发（返回结果设备驱动write的结果），不然写其命令缓冲区（成功返回写入数量（以字节计算），不成功返回-1）
int32_t device_write(struct driver *drv, char *data, uint32_t count);

// 这个注册宏的作用就是把数据放在驱动段（而且告诉编译器这个是被使用的，不能优化掉），它会定义一个驱动结构体实例（增加4字节对齐）
#define REGISTER_DRIVER(drv) \
    __attribute__((section(".drivers"), used, aligned(4))) struct driver drv =



#endif //HOS_KERNEL_DEVICE_H
