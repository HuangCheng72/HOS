//
// Created by Administrator on 2024/6/16.
//

#ifndef HOS_RAMDISK_H
#define HOS_RAMDISK_H

#include "../../lib/lib_kernel/lib_kernel.h"

// 用ramdisk模拟nor闪存的行为


#define RAMDISK_SIZE (16 * 1024 * 1024) // 16MB
#define BLOCK_SIZE (64 * 1024) // NOR闪存块大小，通常为64KB
#define WRITE_GRANULARITY 4 // 写入粒度为4字节（一个字）

int32_t ramdisk_read(char *args, uint32_t args_size);
int32_t ramdisk_write(char *args, uint32_t args_size);

#endif //HOS_RAMDISK_H
