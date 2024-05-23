//
// Created by huangcheng on 2024/5/23.
//

#ifndef HOS_KERNEL_PAGE_H
#define HOS_KERNEL_PAGE_H

#include "../../lib/lib.h"

// 这是一个4位（32字节）的结构体，采用了位域的方式，制定了每一部分占多少位（从低地址到高地址）
// 页目录项 (Page Directory Entry, PDE)（这也是《操作系统真象还原》说的一级页表）
typedef struct {
    uint32_t present : 1;    // 位0: 是否存在 (1=存在, 0=不存在)
    uint32_t rw : 1;         // 位1: 读/写权限 (1=可读写, 0=只读)
    uint32_t us : 1;         // 位2: 用户/系统权限 (1=用户, 0=系统)
    uint32_t pwt : 1;        // 位3: 页面写通 (1=启用, 0=禁用)
    uint32_t pcd : 1;        // 位4: 页面缓存禁用 (1=启用, 0=禁用)
    uint32_t accessed : 1;   // 位5: 访问位 (1=已访问, 0=未访问)
    uint32_t reserved : 1;   // 位6: 保留位 (应为0)
    uint32_t page_size : 1;  // 位7: 页大小 (0=4KB, 1=4MB)
    uint32_t ignored : 1;    // 位8: 被忽略位
    uint32_t available : 3;  // 位9-11: 可用位 (操作系统保留)
    uint32_t table : 20;     // 位12-31: 页表地址 (物理地址右移12位)
} page_directory_entry_t;

// 这是一个4位（32字节）的结构体，采用了位域的方式，制定了每一部分占多少位（从低地址到高地址）
// 页表项 (Page Table Entry, PTE)（这也是《操作系统真象还原》说的二级页表）
typedef struct {
    uint32_t present : 1;    // 位0: 是否存在 (1=存在, 0=不存在)
    uint32_t rw : 1;         // 位1: 读/写权限 (1=可读写, 0=只读)
    uint32_t us : 1;         // 位2: 用户/超级用户权限 (1=用户, 0=超级用户)
    uint32_t pwt : 1;        // 位3: 页面写通 (1=启用, 0=禁用)
    uint32_t pcd : 1;        // 位4: 页面缓存禁用 (1=启用, 0=禁用)
    uint32_t accessed : 1;   // 位5: 访问位 (1=已访问, 0=未访问)
    uint32_t dirty : 1;      // 位6: 脏位 (1=已修改, 0=未修改)
    uint32_t pat : 1;        // 位7: 页面属性表 (1=启用, 0=禁用)
    uint32_t global : 1;     // 位8: 全局页 (1=全局, 0=非全局)
    uint32_t available : 3;  // 位9-11: 可用位 (操作系统保留)
    uint32_t frame : 20;     // 位12-31: 页框地址 (物理地址右移12位)
} page_table_entry_t;

#define PAGE_DIR_TABLE_POS 0x100000  // 页目录表的起始物理地址
#define PAGE_TABLE_ENTRIES 1024      // 每个页表中的页表项数量
#define PAGE_DIR_ENTRIES 1024        // 页目录中的页目录项数量
#define PG_PRESENT 1                 // 页表项/页目录项的存在标志位
#define PG_RW 2                      // 页表项/页目录项的读/写标志位
#define PG_USER 4                    // 页表项/页目录项的用户/超级用户标志位

// 内存分页功能初始化
void init_paging();


#endif //HOS_KERNEL_PAGE_H
