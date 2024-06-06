//
// Created by huangcheng on 2024/5/28.
//

#ifndef HOS_KERNEL_MEMORY_H
#define HOS_KERNEL_MEMORY_H

#include "../kernel_page/kernel_page.h"
#include "../../lib/lib_kernel/lib_kernel.h"

// 区分是内核申请还是非内核申请
typedef enum {
    KERNEL_FLAG = 1,
    USER_FLAG = 2
}MemoryRequesterType;

// 管理所有进程（包括内核的）虚拟地址的结构
typedef struct {
    uint32_t virtual_addr_start;    // 该虚拟地址池管理的虚拟内存起始地址
    BitMap bitmap_virtual_memory;   // 虚拟地址位图
} Virtual_Addr;

// 内存管理初始化
void init_memory(uint32_t total_physical_memory);
// 申请以页为单位的内存
void* malloc_page(MemoryRequesterType type, uint32_t pg_cnt);
// 以页为单位归还内存
void free_page(void* vaddr, uint32_t pg_cnt);


#endif //HOS_KERNEL_MEMORY_H
