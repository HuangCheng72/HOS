//
// Created by huangcheng on 2024/5/27.
//

#include "kernel_device.h"

// 驱动链表头结点
struct list_node driver_list;

// 驱动段的起点和终点
extern struct driver __drivers_start[];
extern struct driver __drivers_end[];

void init_device() {
    // 初始化驱动链表头结点
    init_list_node(&driver_list);
    // 链接到驱动链表
    struct driver* drv;
    for (drv = __drivers_start; drv < __drivers_end; drv++) {
        driver_add(drv);
    }
}
void exit_device() {
    // 移除所有设备
    struct list_node *pos, *n;
    list_for_each_safe(pos, n, &driver_list) {
        // 执行退出程序，然后从链表中移除
        if(list_entry(pos, struct driver, node)->exit != NULL) {
            list_entry(pos, struct driver, node)->exit();
        }
        list_del(pos);
        return;
    }
    // 初始化驱动链表头结点
    init_list_node(&driver_list);
}

// 添加单个驱动
void driver_add(struct driver* drv) {
    if(drv == NULL) {
        return;
    }
    // 防止重复注册
    struct list_node *pos, *n;
    list_for_each_safe(pos, n, &driver_list) {
        if (strcmp(list_entry(pos, struct driver, node)->driver_name, drv->driver_name) == 0) {
            return;
        }
        put_str(drv->driver_name);
        put_char('\n');
    }
    list_add_tail(&drv->node, &driver_list);
    if(drv->init != NULL) {
        drv->init();
    }
}
// 移除单个驱动
void driver_remove(const char *driver_name) {
    struct list_node *pos, *n;
    list_for_each_safe(pos, n, &driver_list) {
        if(strcmp(list_entry(pos, struct driver, node)->driver_name, driver_name) == 0) {
            // 执行退出程序，然后从链表中移除
            if(list_entry(pos, struct driver, node)->exit != NULL) {
                list_entry(pos, struct driver, node)->exit();
            }
            list_del(pos);
            return;
        }
    }
}
