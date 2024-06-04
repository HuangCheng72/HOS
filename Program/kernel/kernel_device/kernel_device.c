//
// Created by huangcheng on 2024/5/27.
//

#include "kernel_device.h"
#include "../kernel_interrupt/kernel_interrupt.h"

// 驱动链表头结点
struct list_node driver_list_head;

// 驱动段的起点和终点
extern struct driver __drivers_start[];
extern struct driver __drivers_end[];

// 中断处理逻辑数组
extern void (*interrupt_handler_functions[256])(void);

void init_all_devices() {
    // 初始化驱动链表头结点
    init_list_node(&driver_list_head);
    // 链接到驱动链表
    struct driver* drv;
    for (drv = __drivers_start; drv < __drivers_end; drv++) {
        driver_add(drv);
    }
}

void exit_all_devices() {
    // 禁用中断
    intr_disable();
    // 遍历驱动链表，执行每个驱动的退出函数
    struct list_node *pos, *n;
    list_for_each_safe(pos, n, &driver_list_head) {
        struct driver *drv = list_entry(pos, struct driver, node);
        if (drv->exit != NULL) {
            drv->exit();
        }

        // 需要内核缓冲区，而且已经被分配了缓冲区的情况
        if (drv->need_kernel_buffer && drv->buffer) {
            // 回收缓冲区
            kernel_buffer_free(drv->buffer);
            // 清空指针
            drv->buffer = NULL;
        }

        // 如果中断号在有效范围内且匹配当前驱动的中断处理程序
        if (drv->irq >= 0 && drv->irq < 16 && interrupt_handler_functions[drv->irq + 0x20] == drv->irq_interrupt_handler) {
            interrupt_handler_functions[drv->irq + 0x20] = NULL;
        }

        // 从链表中移除该驱动
        list_del(pos);
    }
    // 启用中断
    intr_enable();
    // 初始化头结点
    init_list_node(&driver_list_head);
}

void driver_add(struct driver* drv) {
    if (drv == NULL) {
        return;
    }

    // 检查是否重复注册
    struct list_node *pos;
    list_for_each(pos, &driver_list_head) {
        if (strcmp(list_entry(pos, struct driver, node)->driver_name, drv->driver_name) == 0) {
            return;
        }
    }
    // 怕触发中断，所以先关闭中断
    intr_disable();

    if (drv->need_kernel_buffer) {
        // 需要内核缓冲区
        if(drv->buffer) {
            // 为什么你会有内核缓冲区？？无法处理直接return。
            // 重新开启中断
            intr_enable();
            return;
        }
        // 分配一个内核缓冲区
        drv->buffer = kernel_buffer_create();
    }

    // 如果要求注册中断处理函数，必须检查中断号范围
    if (drv->irq >= 0 && drv->irq < 16) {
        // 检查对应位置是否已被占用
        if (interrupt_handler_functions[drv->irq + 0x20] != NULL) {
            // 如果已经分配了缓冲区必须回滚
            if(drv->need_kernel_buffer && drv->buffer) {
                kernel_buffer_free(drv->buffer);
                // 清空指针
                drv->buffer = NULL;
            }
            // 重新开启中断
            intr_enable();
            // 中断处理程序冲突，不得挂载驱动，因而直接return
            return;
        } else {
            interrupt_handler_functions[drv->irq + 0x20] = drv->irq_interrupt_handler;
        }
    } else if (drv->irq_interrupt_handler != NULL) {
        // 如果已经分配了缓冲区必须回滚
        if(drv->need_kernel_buffer && drv->buffer) {
            kernel_buffer_free(drv->buffer);
            // 清空指针
            drv->buffer = NULL;
        }
        // 重新开启中断
        intr_enable();
        // IRQ 不在范围内但要求注册中断处理函数，不得挂载驱动，因而直接return
        return;
    }

    // 执行初始化函数
    if (drv->init != NULL) {
        drv->init();
    }

    // 现在不怕触发中断了就可以打开中断
    intr_enable();

    // 将驱动添加到驱动链表
    list_add_tail(&drv->node, &driver_list_head);
}

void driver_remove(const char *driver_name) {
    struct list_node *pos, *n;
    list_for_each_safe(pos, n, &driver_list_head) {
        struct driver *drv = list_entry(pos, struct driver, node);
        if (strcmp(drv->driver_name, driver_name) == 0) {
            // 首先禁止中断，怕又触发中断
            intr_disable();

            // 执行退出函数
            if (drv->exit != NULL) {
                drv->exit();
            }

            // 需要内核缓冲区，而且已经被分配了缓冲区的情况
            if (drv->need_kernel_buffer && drv->buffer) {
                // 回收缓冲区
                kernel_buffer_free(drv->buffer);
                // 清空指针
                drv->buffer = NULL;
            }

            // 检查是否需要清除中断处理逻辑
            if (drv->irq >= 0 && drv->irq < 16 && interrupt_handler_functions[drv->irq + 0x20] == drv->irq_interrupt_handler) {
                interrupt_handler_functions[drv->irq + 0x20] = NULL;
            }

            // 清除干净，允许中断
            intr_enable();

            // 从链表中移除该驱动
            list_del(pos);
            return;
        }
    }
}

// 获取设备驱动
struct driver* get_driver(const char *driver_name) {
    if(strlen(driver_name) > 63) {
        // 名字太长存不下，肯定找不到
        return NULL;
    }
    struct list_node *pos;
    list_for_each(pos, &driver_list_head) {
        if (strcmp(list_entry(pos, struct driver, node)->driver_name, driver_name) == 0) {
            // 找到驱动结构体实例，返回其指针
            return list_entry(pos, struct driver, node);
        }
    }
    // 找不到那就只能null了
    return NULL;
}
// 对设备（驱动）读，本质上就是读其缓冲区（成功返回读出数量（以字节计算），不成功返回-1）
int32_t device_read(struct driver *drv, char *data, uint32_t count) {
    if(!drv || !(drv->buffer)) {
        return -1;
    }
    kernel_buffer_read(drv->buffer, data, count);
    return count;
}
// 对设备（驱动）写，本质上就是写其缓冲区（成功返回写入数量（以字节计算），不成功返回-1）
int32_t device_write(struct driver *drv, char *data, uint32_t count) {
    if(!drv || !(drv->buffer)) {
        return -1;
    }
    kernel_buffer_write(drv->buffer, data, count);
    return count;
}
