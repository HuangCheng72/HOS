//
// Created by Administrator on 2024/6/16.
//

#ifndef HOS_RAMDISK_H
#define HOS_RAMDISK_H

#include "../../lib/lib_kernel/lib_kernel.h"

// 设备大小
#define RAMDISK_SIZE (16 * 1024 * 1024) // 16MB
// 一个扇区的大小
#define SECTOR_SIZE 512

// 读取和写入操作标志
#define READ_CMD 0x52454144 // ASCII "READ"
#define WRITE_CMD 0x57524954 // ASCII "WRIT"
#define DELETE_CMD 0x44454C45 // ASCII "DELE"

int32_t ramdisk_read(int sector, void *buffer, uint32_t size);
int32_t ramdisk_write(int sector, const void *buffer, uint32_t size);
void ramdisk_task_function();

#endif //HOS_RAMDISK_H
