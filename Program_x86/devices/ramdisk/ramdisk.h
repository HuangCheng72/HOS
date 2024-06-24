//
// Created by Administrator on 2024/6/16.
//

#ifndef HOS_RAMDISK_H
#define HOS_RAMDISK_H

#include "../../lib/lib_kernel/lib_kernel.h"

#define RAMDISK_SIZE (16 * 1024 * 1024) // 16MB

int32_t ramdisk_read(char *args, uint32_t args_size);
int32_t ramdisk_write(char *args, uint32_t args_size);

#endif //HOS_RAMDISK_H
