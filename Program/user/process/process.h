//
// Created by huangcheng on 2024/6/5.
//

#ifndef HOS_PROCESS_H
#define HOS_PROCESS_H
#include "../../kernel/kernel_task/kernel_task.h"

// 这些是默认的约定
// 用户栈的栈底是0xc0000000往前一页，保证绝对干涉不到内核虚拟地址
#define USER_STACK3_VADDR  (0xc0000000 - 0x1000)
// 0x8048000是Linux的ELF格式可执行文件的默认入口地址（本身已经是128MB处了）
// 所以以此作为进程虚拟空间的起点（其实定什么都行，避开已经使用的虚拟地址就行）
#define USER_VADDR_START 0x8048000


// 创建一个用户进程
// 参数为进程名和入口函数
struct task* process_create(char *name, uint32_t entry_function);

#endif //HOS_PROCESS_H
