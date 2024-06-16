//
// Created by huangcheng on 2024/5/23.
//

#include "kernel_page.h"

void setup_page_directory(page_directory_entry_t* page_directory) {
    // 清空整个页目录表
    memset(page_directory, 0, 0x1000);

    // 第一个页表放低端4MB映射
    page_table_entry_t* first_page_table = (page_table_entry_t*) ((uint32_t)page_directory + 0x1000);

    // 初始化1024个页表条目（覆盖低端4MB，包括了整个位图的区域）
    for (int i = 0; i < 1024; ++i) {
        // 设置页表项 (PTE)
        first_page_table[i].present = 1;     // 页表项存在
        first_page_table[i].rw = 1;          // 页表项可读写
        first_page_table[i].us = 0;          // 页表项为超级用户权限

        first_page_table[i].frame = (i * 0x1000) >> 12;       // 页框地址，物理地址右移12位
        // 等效于 first_page_table[i].frame = i，但这才是完整的计算逻辑
    }
    // 设置页目录表项
    page_directory[0].present = 1;     // 页目录表项存在
    page_directory[0].rw = 1;          // 页目录表项可读写
    page_directory[0].us = 0;          // 页目录表项为超级用户权限
    page_directory[0].table = (uint32_t)first_page_table >> 12;       // 页表地址，物理地址右移12位

    // 设置页目录表的第768个条目，映射高地址内存 (从3GB开始)
    // 说清楚就是，768 * 1024 = 3GB = 0xc0000000，从这里开始，低地址和高地址可以很轻易计算了
    // 这里需要再用一页
    page_table_entry_t* second_page_table = (page_table_entry_t*) ((uint32_t)first_page_table + 0x1000);
    // 初始化256个页表条目（对应低端1MB）
    for (int i = 0; i < 256; ++i) {
        // 设置页表项 (PTE)
        second_page_table[i].present = 1;     // 页表项存在
        second_page_table[i].rw = 1;          // 页表项可读写
        second_page_table[i].us = 0;          // 页表项为超级用户权限

        second_page_table[i].frame = (i * 0x1000) >> 12;       // 页框地址，物理地址右移12位
    }
    page_directory[768].present = 1;     // 页目录表项存在
    page_directory[768].rw = 1;          // 页目录表项可读写
    page_directory[768].us = 0;          // 页目录表项为超级用户权限
    page_directory[768].table = ((uint32_t)second_page_table) >> 12;  // 页表地址

    // 页目录表最后一项自引用
    // 其作用是快速寻址到页目录表
    // 只要最后一项自引用了，0xfffff000就是当前页目录表所在的地方，找页目录表地址就很方便了，直接变成了常量
    page_directory[1023].present = 1;     // 页表项存在
    page_directory[1023].rw = 1;          // 页表项可读写
    page_directory[1023].us = 0;          // 页表项为超级用户权限
    page_directory[1023].table = (uint32_t)page_directory >> 12;  // 页表地址

    // 添加新的页表，映射48MB到64MB的物理内存到0xA0000000处（用于虚拟设备ramdisk）
    // 由于需要映射ramdisk设备的内存，需要建立4页的页表。
    // 怕冲突其他的，所以塞在0x122000之后，0x130000之前，又不干涉内核虚拟位图（内核虚拟位图从0x122000开始连续8页），所以起点为0x12a000，满四页正好到0x12efff
    page_table_entry_t* ramdisk_page_table = (page_table_entry_t*)0x12a000;
    for (int i = 0; i < 4096; ++i) { // 16MB / 4KB = 4096 个页表项，一个页表项4字节，需要4页
        ramdisk_page_table[i].present = 1;     // 页表项存在
        ramdisk_page_table[i].rw = 1;          // 页表项可读写
        ramdisk_page_table[i].us = 0;          // 页表项为超级用户权限
        ramdisk_page_table[i].frame = (0x03000000 >> 12) + i; // 页框地址，物理地址右移12位
    }

    // 设置页目录表项，将其映射到0xA0000000
    for (int i = 0; i < 4; i++) {
        page_directory[640 + i].present = 1;     // 页目录表项存在
        page_directory[640 + i].rw = 1;          // 页目录表项可读写
        page_directory[640 + i].us = 0;          // 页目录表项为超级用户权限
        page_directory[640 + i].table = ((0x12a000 + i * 0x1000) >> 12); // 页表地址
    }
}


void init_paging() {
    // 明确页目录表的起始地址是0x100000（1MB处）
    page_directory_entry_t* page_directory = (page_directory_entry_t*) PAGE_DIR_TABLE_POS;
    // 设置页目录表
    setup_page_directory(page_directory);
    // 载入内核的页目录表，这里要寄存器操作，需要用汇编写
    load_page_directory(PAGE_DIR_TABLE_POS);
    // 启用分页
    enable_paging();
}
