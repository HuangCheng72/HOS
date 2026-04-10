//
// Created by huangcheng on 2024/5/27.
//

#include "kernel_device.h"
#include "../kernel_interrupt/kernel_interrupt.h"

// 驱动链表头结点
struct list_node driver_list_head;

// 驱动段的起点和终点
extern struct driver_descriptor __drivers_start[];
extern struct driver_descriptor __drivers_end[];

// 注册一个驱动的全部 IRQ
// 全部注册成功返回1，失败返回0
// 如果中途失败，会自动回滚之前已经注册成功的 IRQ
uint8_t register_driver_irqs(struct driver_descriptor *drv) {
    if (drv == NULL) {
        return 0;
    }

    // 没有 IRQ 也算成功
    if (drv->irq_descriptors == NULL || drv->irq_count == 0) {
        return 1;
    }

    uint32_t i;
    for (i = 0; i < drv->irq_count; i++) {
        // IRQ 号无效，直接视为失败
        if (drv->irq_descriptors[i].irq < 0) {
            break;
        }

        // 中断处理函数为空，直接视为失败
        if (drv->irq_descriptors[i].irq_interrupt_handler == NULL) {
            break;
        }

        // 尝试注册中断处理函数，失败则跳出回滚
        if (add_interrupt_handler(drv->irq_descriptors[i].irq,
                                  drv->irq_descriptors[i].irq_interrupt_handler,
                                  drv->irq_descriptors[i].trigger_mode)
                                  == false) {
            break;
        }
    }

    // 全部注册成功
    if (i == drv->irq_count) {
        return 1;
    }

    // 中途失败，回滚前面已经注册成功的 IRQ
    while (i > 0) {
        i--;
        remove_interrupt_handler(drv->irq_descriptors[i].irq);
    }

    return 0;
}


// 移除一个驱动的全部 IRQ
void unregister_driver_irqs(struct driver_descriptor *drv) {
    if (drv == NULL) {
        return;
    }

    if (drv->irq_descriptors == NULL || drv->irq_count == 0) {
        return;
    }

    uint32_t i;
    for (i = 0; i < drv->irq_count; i++) {
        if (drv->irq_descriptors[i].irq >= 0) {
            remove_interrupt_handler(drv->irq_descriptors[i].irq);
        }
    }
}


// 初始化所有设备
void init_all_devices() {
    // 禁用中断
    intr_disable();

    // 初始化驱动链表头结点
    init_list_node(&driver_list_head);

    // 启用中断
    intr_enable();

    // 链接到驱动链表
    struct driver_descriptor* drv;
    for (drv = __drivers_start; drv < __drivers_end; drv++) {
        driver_add(drv);
    }
}


// 移除所有设备
void exit_all_devices() {
    // 禁用中断
    intr_disable();

    // 遍历驱动链表，执行每个驱动的退出函数
    struct list_node *pos, *n;
    list_for_each_safe(pos, n, &driver_list_head) {
        struct driver_descriptor *drv = list_entry(pos, struct driver_descriptor, node);

        // 执行退出函数
        if (drv->exit != NULL) {
            drv->exit();
        }

        // 移除这个驱动的全部 IRQ
        unregister_driver_irqs(drv);

        // 从链表中移除该驱动
        list_del(pos);
    }

    // 重新初始化头结点
    init_list_node(&driver_list_head);

    // 启用中断
    intr_enable();
}


// 添加单个驱动并初始化
void driver_add(struct driver_descriptor* drv) {
    if (drv == NULL) {
        return;
    }

    // 检查是否重复注册
    struct list_node *pos;
    list_for_each(pos, &driver_list_head) {
        if (strcmp(list_entry(pos, struct driver_descriptor, node)->driver_name, drv->driver_name) == 0) {
            return;
        }
    }

    // 怕触发中断，所以先关闭中断
    intr_disable();

    // 先注册这个驱动的全部 IRQ
    // 如果失败，说明中断处理程序冲突或者描述符本身有问题，直接退出
    if (register_driver_irqs(drv) == 0) {
        intr_enable();
        return;
    }

    // 执行初始化函数
    if (drv->init != NULL) {
        drv->init();
    }

    // 将驱动添加到驱动链表
    list_add_tail(&drv->node, &driver_list_head);

    // 现在不怕触发中断了就可以打开中断
    intr_enable();
}


// 移除单个驱动并卸载
void driver_remove(const char *driver_name) {
    if (driver_name == NULL) {
        return;
    }

    if (strlen(driver_name) > 63) {
        // 名字太长存不下，肯定找不到
        return;
    }

    struct list_node *pos, *n;
    list_for_each_safe(pos, n, &driver_list_head) {
        struct driver_descriptor *drv = list_entry(pos, struct driver_descriptor, node);

        if (strcmp(drv->driver_name, driver_name) == 0) {
            // 首先禁止中断，怕又触发中断
            intr_disable();

            // 执行退出函数
            if (drv->exit != NULL) {
                drv->exit();
            }

            // 移除这个驱动的全部 IRQ
            unregister_driver_irqs(drv);

            // 从链表中移除该驱动
            list_del(pos);

            // 清除干净，允许中断
            intr_enable();

            return;
        }
    }
}


// 获取设备驱动
struct driver_descriptor* get_driver(const char *driver_name) {
    if (driver_name == NULL) {
        return NULL;
    }

    if (strlen(driver_name) > 63) {
        // 名字太长存不下，肯定找不到
        return NULL;
    }

    struct list_node *pos;
    list_for_each(pos, &driver_list_head) {
        struct driver_descriptor *drv = list_entry(pos, struct driver_descriptor, node);

        if (strcmp(drv->driver_name, driver_name) == 0) {
            // 找到驱动描述符实例，返回其指针
            return drv;
        }
    }

    // 找不到那就只能NULL了
    return NULL;
}
