//
// Created by huangcheng on 2024/5/27.
//

#ifndef HOS_KERNEL_DEVICE_H
#define HOS_KERNEL_DEVICE_H

#include "kernel_device_common.h"

// 初始化所有设备
void init_all_devices();
// 移除所有设备
void exit_all_devices();
// 添加单个驱动并初始化
void driver_add(struct driver_descriptor* drv);
// 移除单个驱动并卸载
void driver_remove(const char *driver_name);
// 获取设备驱动
struct driver_descriptor* get_driver(const char *driver_name);

#endif //HOS_KERNEL_DEVICE_H
