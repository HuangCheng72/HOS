//
// Created by huangcheng on 2024/5/23.
//

#ifndef HOS_LIB_H
#define HOS_LIB_H

#include "type.h"

// 这个文件存在的意义主要就是为了声明汇编暴露的函数，加上type.h可以为系统各组件提供统一的支持

// 载入页目录
extern void load_page_directory(uint32_t* page_directory);
// 启用分页
extern void enable_paging();
// 保存GDT
extern void store_gdt(uint32_t gdt_ptr_addr);
// 加载GDT
extern void load_gdt(uint32_t gdt_ptr_addr);
// 设置esp
extern void switch_esp_virtual_addr();

#endif //HOS_LIB_H
