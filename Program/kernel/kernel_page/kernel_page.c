//
// Created by huangcheng on 2024/5/23.
//

#include "kernel_page.h"

void setup_page_directory(page_directory_entry_t* page_directory) {
    // 初始化页目录表的所有条目（全部为0）
    for (int i = 0; i < PAGE_DIR_ENTRIES; ++i) {
        // 初始化页目录项 (PDE)
        page_directory[i].present = 0;       // 页目录项不存在
        page_directory[i].rw = 0;            // 页目录项可读写
        page_directory[i].us = 0;            // 页目录项为系统权限
        page_directory[i].pwt = 0;           // 禁用页面写通
        page_directory[i].pcd = 0;           // 禁用页面缓存
        page_directory[i].accessed = 0;      // 页面未被访问
        page_directory[i].reserved = 0;      // 保留位，必须为0
        page_directory[i].page_size = 0;     // 页面大小为4KB
        page_directory[i].ignored = 0;       // 被忽略位
        page_directory[i].available = 0;     // 可用位，操作系统保留
        page_directory[i].table = 0;         // 页表地址
    }

    // 1024个页目录表，每个页目录表都是4字节（1字节8位，一共32位）
    // 那么页目录表占据1024 * 4 = 4096字节，十六进制换算为0x1000。
    // 第一个页表项的位置紧贴着页目录表结束的位置，因此 从PAGE_DIR_TABLE_POS + 0x1000 处开始
    // PAGE_DIR_TABLE_POS 是 0x100000，也就是1MB
    // 页表也是一样的，4096字节，4KB大小，也就是说从1MB开始的两个内存页，就是我们存放分页文件的地方
    page_table_entry_t* first_page_table = (page_table_entry_t*) (PAGE_DIR_TABLE_POS + 0x1000);

    // 初始化第一个256个页表条目（对应低端1MB，内核已使用）
    for (int i = 0; i < 256; ++i) {
        // 设置页表项 (PTE)
        first_page_table[i].present = 1;     // 页表项存在
        first_page_table[i].rw = 1;          // 页表项可读写
        first_page_table[i].us = 0;          // 页表项为超级用户权限

        first_page_table[i].frame = (i * 0x1000) >> 12;       // 页框地址，物理地址右移12位
        // 等效于 first_page_table[i].frame = i，但这才是完整的计算逻辑
    }

    // 设置页目录表的第一个条目，映射低端内存 (第一个1MB)
    page_directory[0].present = 1;           // 页目录项存在
    page_directory[0].rw = 1;                // 页目录项可读写
    page_directory[0].us = 0;                // 页目录项为超级用户权限
    page_directory[0].table = ((uint32_t) first_page_table) >> 12;  // 页表地址

    // 设置页目录表的第768个条目，映射高地址内存 (从3GB开始)
    // 说清楚就是，768 * 1024 = 3GB = 0xc0000000，从这里开始，低地址和高地址可以很轻易计算了

    // 第一个1MB
    page_directory[768].present = 1;         // 页目录项存在
    page_directory[768].rw = 1;              // 页目录项可读写
    page_directory[768].us = 0;              // 页目录项为超级用户权限
    page_directory[768].table = ((uint32_t) first_page_table) >> 12;  // 页表地址


    // 设置页目录表的最后一个条目，指向页目录表自身，实现自引用
    page_directory[1023].present = 1;        // 页目录项存在
    page_directory[1023].rw = 1;             // 页目录项可读写
    page_directory[1023].us = 0;             // 页目录项为超级用户权限
    page_directory[1023].table = ((uint32_t) page_directory) >> 12;  // 页目录地址


    // 设置页目录表第770到1023个条目，映射高地址内存 (从3GB开始)
    for (int i = 1; i < 256; ++i) {
        page_directory[768 + i].present = 1;     // 页目录项存在
        page_directory[768 + i].rw = 1;          // 页目录项可读写
        page_directory[768 + i].us = 0;          // 页目录项为用户权限
        // 页表地址，偏移量 i * 0x1000 表示页表的顺序排列
        page_directory[768 + i].table = (((uint32_t) first_page_table) + i * 0x1000) >> 12;
    }
}


void init_paging() {
    // 明确页目录表的起始地址是0x100000（1MB处）
    page_directory_entry_t* page_directory = (page_directory_entry_t*) PAGE_DIR_TABLE_POS;
    // 设置页目录表
    setup_page_directory(page_directory);
    // 载入页目录表，这里要寄存器操作，需要用汇编写
    load_page_directory((uint32_t*) page_directory);
    // 启用分页
    enable_paging();
}
