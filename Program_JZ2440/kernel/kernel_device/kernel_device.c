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

        // 如果中断号在有效范围内就移除中断处理函数
        if(drv->irq >= 0) {
            remove_interrupt_handler(drv->irq);
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
    if (drv->irq >= 0) {
        // 尝试注册中断处理函数
        if(add_interrupt_handler(drv->irq, drv->irq_interrupt_handler) == false) {
            // 失败了就要回滚
            free_driver_buffer(drv);    // 回滚缓冲区
            // 重新开启中断
            intr_enable();
            // 中断处理程序冲突，不得挂载驱动，因而直接return
            return;
        }
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
            free_driver_buffer(drv);

            // 尝试移除中断处理逻辑
            if (drv->irq >= 0) {
                remove_interrupt_handler(drv->irq);
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
// 对设备（驱动）读，如果设备驱动提供了read就转发（返回结果设备驱动read的结果），不然读其数据缓冲区（成功返回读出数量（以字节计算），不成功返回-1）
int32_t device_read(struct driver *drv, char *data, uint32_t count) {
    if(!drv) {
        return -1;
    }
    // 什么都不提供，自然要失败
    if(drv->read == NULL && drv->data_buffer == NULL) {
        return -1;
    }

    // 驱动提供的函数优先级更高
    if(drv->read) {
        // 直接转发，拆包参数由驱动自行解决
        return drv->read(data, count);
    }

    // 没提供函数的情况下，直接读缓冲区了
    if(drv->data_buffer) {
        uint32_t total_read = 0;
        uint32_t to_read = 0;
        uint32_t chunk_size = 3072; // 限制一次性读3KB

        while (total_read < count) {
            to_read = (count - total_read > chunk_size) ? chunk_size : count - total_read;
            kernel_buffer_read(drv->data_buffer, data + total_read, to_read);
            total_read += to_read;
        }

        return total_read;
    }
    // 理论上应该不可能跳到这里，还是预防万一
    return -1;
}
// 对设备（驱动）写，如果设备驱动提供了write就转发（返回结果设备驱动write的结果），不然写其命令缓冲区（成功返回写入数量（以字节计算），不成功返回-1）
int32_t device_write(struct driver *drv, char *data, uint32_t count) {
    if(!drv) {
        return -1;
    }

    // 什么都不提供，自然要失败
    if(drv->write == NULL && drv->command_buffer == NULL) {
        return -1;
    }

    // 驱动提供的函数优先级更高
    if(drv->write) {
        // 直接转发，拆包参数由驱动自行解决
        return drv->write(data, count);
    }

    // 没提供函数的情况下，直接读缓冲区了
    if(drv->command_buffer) {
        uint32_t total_written = 0;
        uint32_t to_write = 0;
        uint32_t chunk_size = 3072; // 限制一次性写3KB

        while (total_written < count) {
            to_write = (count - total_written > chunk_size) ? chunk_size : count - total_written;
            kernel_buffer_write(drv->command_buffer, data + total_written, to_write);
            total_written += to_write;
        }

        return total_written;
    }
    // 理论上应该不可能跳到这里，还是预防万一
    return -1;
}
