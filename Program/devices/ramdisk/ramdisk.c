//
// Created by Administrator on 2024/6/16.
//

#include "ramdisk.h"
#include "../../kernel/kernel_device/kernel_device.h"
#include "../console/console.h"
#include "../../kernel/kernel_task/kernel_task.h"

// 注册驱动结构体
REGISTER_DRIVER(ramdisk_driver) {
        .driver_name = "ramdisk",
        .init = NULL,
        .exit = NULL,
        .read = ramdisk_read,
        .write = ramdisk_write,
        .irq = -1, // 没有中断处理
        .irq_interrupt_handler = NULL,
};

// 仅限此处使用，所以要用static，限制作用域在此文件中
static uint8_t *ramdisk = (uint8_t *)0xA0000000; // 定义RAM Disk的存储空间

struct ramdisk_io_request {
    uint32_t sector;
    uint32_t size;
    char *buffer;   // 指向具体数据的指针
};

int32_t ramdisk_read(char *args, uint32_t args_size) {
    if(args_size != sizeof(struct ramdisk_io_request)) {
        // 简单校验是否是本设备能使用的参数包
        return -1;
    }
    // 拆包参数
    struct ramdisk_io_request *request = (struct ramdisk_io_request *)args;

    if (request->sector * SECTOR_SIZE + request->size > RAMDISK_SIZE) {
        return -1; // 超出范围
    }
    // 具体操作数据
    memcpy(request->buffer, ramdisk + request->sector * SECTOR_SIZE, request->size);
    return request->size;
}

int32_t ramdisk_write(char *args, uint32_t args_size) {
    if(args_size != sizeof(struct ramdisk_io_request)) {
        // 简单校验是否是本设备能使用的参数包
        return -1;
    }
    // 拆包参数
    struct ramdisk_io_request *request = (struct ramdisk_io_request *)args;

    if (request->sector * SECTOR_SIZE + request->size > RAMDISK_SIZE) {
        return -1; // 超出范围
    }
    // 具体操作数据
    memcpy(ramdisk + request->sector * SECTOR_SIZE, request->buffer, request->size);
    return request->size;
}
