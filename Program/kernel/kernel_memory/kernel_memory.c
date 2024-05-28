//
// Created by huangcheng on 2024/5/28.
//

#include "kernel_memory.h"

uint32_t kernel_pages;
uint32_t user_pages;

BitMap bitmap_memory;

// 内存管理初始化
void init_memory() {
    // 最少8MB，8MB等于2048页（暂不考虑扩充事项）
    kernel_pages = 2048;
    // 之前计算多少字节，现在直接换算成页（4K）
    user_pages = *((uint32_t *)(0xa09)) / PG_SIZE - kernel_pages;

    bitmap_memory.bits = (uint8_t *)BITMAP_BASE;
    // 一页是一位，一字节8位
    bitmap_memory.btmp_bytes_len = (kernel_pages + user_pages) / 8;

    // 初始化位图
    bitmap_init(&bitmap_memory);

    // 0 到 0x1ffff 是留给内核的，0x20000到0x3ffff是留给位图的
    bitmap_set_range(&bitmap_memory, 0, 0x40000 / PG_SIZE, 1);
    // 0x100000到0x500fff是留给页表的
    bitmap_set_range(&bitmap_memory, 0x100000 / PG_SIZE , (0x501000 - 0x100000) / PG_SIZE, 1);

}
// 申请以页为单位的内存
void* malloc_page(enum pool_flags pf, uint32_t pg_cnt){
    if(pf == PF_USER) {
        return NULL;
    }
    int bit_idx_start = bitmap_scan(&bitmap_memory, pg_cnt);  // 扫描位图，找到连续的空闲页
    if (bit_idx_start == -1) {
        return NULL;  // 没有足够的连续空闲页
    }

    bitmap_set_range(&bitmap_memory, bit_idx_start, pg_cnt, 1); // 标记这些页为已使用

    uint32_t vaddr_start = bit_idx_start * PG_SIZE + 0xc0000000;  // 计算虚拟地址
    return (void*)vaddr_start;

}
// 以页为单位归还内存
void free_page(enum pool_flags pf, void *ptr, uint32_t pg_cnt) {
    if(pf == PF_USER) {
        return;
    }

    uint32_t vaddr_start = (uint32_t)ptr;
    uint32_t bit_idx_start = (vaddr_start - 0xc0000000) / PG_SIZE;  // 计算位图索引

    bitmap_set_range(&bitmap_memory, bit_idx_start, pg_cnt, 0); // 标记这些页为未使用

}