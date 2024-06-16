//
// Created by huangcheng on 2024/5/27.
//

#include "kernel_device.h"
#include "../kernel_interrupt/kernel_interrupt.h"
#include "../kernel_task/kernel_task.h"

// 驱动链表头结点
struct list_node driver_list_head;

// 驱动段的起点和终点
extern struct driver __drivers_start[];
extern struct driver __drivers_end[];

// 中断处理逻辑数组
extern void (*interrupt_handler_functions[256])(void);

// 回收缓冲区
void free_driver_buffer(struct driver *drv) {

    // 需要内核缓冲区，而且已经被分配了缓冲区的情况
    if (drv->need_data_buffer && drv->data_buffer) {
        // 回收缓冲区
        kernel_buffer_free(drv->data_buffer);
        // 清空指针
        drv->data_buffer = NULL;
    }

    // 需要内核缓冲区，而且已经被分配了缓冲区的情况
    if (drv->need_command_buffer && drv->command_buffer) {
        // 回收缓冲区
        kernel_buffer_free(drv->command_buffer);
        // 清空指针
        drv->command_buffer = NULL;
    }

}

// 分配缓冲区，原子操作，失败回滚，返回1为成功，0为失败
uint8_t alloc_driver_buffer(struct driver *drv) {
    struct kernel_buffer *data_buf = NULL;
    struct kernel_buffer *command_buf = NULL;

    // 尝试分配数据缓冲区
    if (drv->need_data_buffer) {
        if (drv->data_buffer) {
            // 如果已经存在数据缓冲区，直接返回，不做任何更改
            return 0;
        } else {
            data_buf = kernel_buffer_create();
            if (!data_buf) {
                // 分配失败，直接返回，不做任何更改
                return 0;
            }
        }
    }

    // 尝试分配命令缓冲区
    if (drv->need_command_buffer) {
        if (drv->command_buffer) {
            // 如果已经存在命令缓冲区，先释放已分配的数据缓冲区（如果有的话）
            if (data_buf) {
                kernel_buffer_free(data_buf);
            }
            // 直接返回，不做任何更改
            return 0;
        } else {
            command_buf = kernel_buffer_create();
            if (!command_buf) {
                // 分配失败，回滚已分配的数据缓冲区（如果有的话）
                if (data_buf) {
                    kernel_buffer_free(data_buf);
                }
                // 直接返回，不做任何更改
                return 0;
            }
        }
    }

    // 如果成功分配了缓冲区，更新驱动的缓冲区指针
    // 这里的指针如果不为空，肯定说明要求分配了，所以直接更新即可，不用再次判定是否要求分配了
    if (data_buf) {
        drv->data_buffer = data_buf;
    }
    if (command_buf) {
        drv->command_buffer = command_buf;
    }

    return 1;
}

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
        free_driver_buffer(drv);

        // 如果中断号在有效范围内且匹配当前驱动的中断处理程序
        if (drv->irq >= 0 && drv->irq < 16 && interrupt_handler_functions[drv->irq + 0x20] == drv->irq_interrupt_handler) {
            interrupt_handler_functions[drv->irq + 0x20] = NULL;
        }

        if(drv->driver_task_function != NULL) {
            // 回收内核任务
            task_cancel(task_info(drv->driver_name));
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

    // 分配缓冲区，失败就直接退出
    if(alloc_driver_buffer(drv) == 0) {
        return;
    }

    // 怕触发中断，所以先关闭中断
    intr_disable();

    // 如果要求注册中断处理函数，必须检查中断号范围
    if (drv->irq >= 0 && drv->irq < 16) {
        // 检查对应位置是否已被占用
        if (interrupt_handler_functions[drv->irq + 0x20] != NULL) {
            // 如果已经分配了缓冲区必须回滚
            free_driver_buffer(drv);
            // 重新开启中断
            intr_enable();
            // 中断处理程序冲突，不得挂载驱动，因而直接return
            return;
        } else {
            interrupt_handler_functions[drv->irq + 0x20] = drv->irq_interrupt_handler;
        }
    } else if (drv->irq_interrupt_handler != NULL) {
        // 如果已经分配了缓冲区必须回滚
        free_driver_buffer(drv);
        // 重新开启中断
        intr_enable();
        // IRQ 不在范围内但要求注册中断处理函数，不得挂载驱动，因而直接return
        return;
    }

    // 执行初始化函数
    if (drv->init != NULL) {
        drv->init();
    }

    if(drv->driver_task_function != NULL) {
        // 以设备名建立一个任务（反正是两个不同的体系，不怕冲突）
        task_create(drv->driver_name, 31, drv->driver_task_function, drv->driver_task_function_args);
    }

    // 现在不怕触发中断了就可以打开中断
    intr_enable();

    // 将驱动添加到驱动链表
    list_add_tail(&drv->node, &driver_list_head);
}

void driver_remove(const char *driver_name) {
    if(strlen(driver_name) > 63) {
        // 名字太长存不下，肯定找不到
        return;
    }

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
            alloc_driver_buffer(drv);

            // 检查是否需要清除中断处理逻辑
            if (drv->irq >= 0 && drv->irq < 16 && interrupt_handler_functions[drv->irq + 0x20] == drv->irq_interrupt_handler) {
                interrupt_handler_functions[drv->irq + 0x20] = NULL;
            }

            if(drv->driver_task_function != NULL) {
                // 回收内核任务
                task_cancel(task_info(drv->driver_name));
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
// 对设备（驱动）读，本质上就是读其数据缓冲区（成功返回读出数量（以字节计算），不成功返回-1）
int32_t device_read(struct driver *drv, char *data, uint32_t count) {
    if(!drv || !(drv->data_buffer)) {
        return -1;
    }
    kernel_buffer_read(drv->data_buffer, data, count);
    return count;
}
// 对设备（驱动）写，本质上就是写其命令缓冲区（成功返回写入数量（以字节计算），不成功返回-1）
int32_t device_write(struct driver *drv, char *data, uint32_t count) {
    if(!drv || !(drv->command_buffer)) {
        return -1;
    }
    kernel_buffer_write(drv->command_buffer, data, count);
    return count;
}
