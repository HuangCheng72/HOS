//
// Created by huangcheng on 2024/5/23.
//

#ifndef HOS_LIB_H
#define HOS_LIB_H

#include "../type.h"
#include "print/print.h"
#include "string/string.h"
#include "bitmap/bitmap.h"
#include "list/list.h"

// 这个文件存在的意义主要就是为了声明汇编暴露的函数，加上type.h可以为系统各组件提供统一的支持

// 载入页目录表到ttbr0和ttbr1，paging_ops.asm
extern void load_page_directory(uint32_t page_directory_addr);
// 获取TTBR0寄存器中的当前页表，paging_ops.asm
extern uint32_t get_current_page(void);
// 切换栈指针
extern void switch_sp(uint32_t new_sp);
// 延迟给定纳秒数
extern void delay(uint32_t us);

#endif //HOS_LIB_H
