//
// Created by huangcheng on 2024/5/28.
//

#ifndef HOS_KERNEL_MEMORY_H
#define HOS_KERNEL_MEMORY_H

#include "../kernel_page/kernel_page.h"
#include "../../lib/lib_kernel/lib_kernel.h"

// 目前的内存分配设想
// 1. 0到0x900作为栈使用
// 2. 0x900（Loader入口）到0x1500（内核入口）虽然用过了，但是里面还有个内存容量数据，不能覆盖
// 3. 0x100000是页目录表起点，如果管理的内存是4GB，页目录表+页表要4KB+4MB
// 4. 内核无论如何预留最少8MB，根据内存容量来，如果内存容量大，就收多一点，但是无论再小起码8MB，不然不够用。
// 所以，0到0x7fffff是内核的堆内存，0x800000开始才是用户的堆内存
// 其中内核内存池0到0x1ffff标记为已使用（64KB，很充足了）。0x100000到0x500fff标记为已使用
// 如果内存容量超过3GB，接触到映射低端内存的虚拟地址了。用户内存池初始化的时候要跳过这一段。
// 管理4G内存的位图，至少要128KB。因此要划分出一块区域，选择0x20000作为起点，到0x3ffff为终点
// 内核堆内存包括两部分，一部分是0x40000到0xfffff（后续有占据了的话再缩小范围就是了），一部分是0x501000到0x7fffff


// 位图基址
#define BITMAP_BASE 0x20000
// 一页大小4096字节，4KB
#define PG_SIZE 4096

// 区分用的是哪个内存池
enum pool_flags {
    PF_KERNEL = 1,
    PF_USER = 2
};

// 内存管理初始化
void init_memory();
// 申请以页为单位的内存
void* malloc_page(enum pool_flags pf, uint32_t pg_cnt);
// 以页为单位归还内存
void free_page(enum pool_flags pf, void *ptr, uint32_t pg_cnt);


#endif //HOS_KERNEL_MEMORY_H
