//
// Created by huangcheng on 2024/6/8.
//

#ifndef HOS_SYSCALL_H
#define HOS_SYSCALL_H

#include "../type.h"

// 可以用枚举类型来列举定义具体是哪个系统调用，用整数也可以，总之约定好就行
// 枚举比如
/*
enum SYSCALL_TYPE {
    SYS_GETPID,
    SYS_WRITE,
    SYS_MALLOC,
    SYS_FREE
};
 */
// 我用整数，觉得比较方便，编个号就行了

// 用户版本的put_str
uint32_t write(char* str);
// 获得当前任务的名字
void get_process_name(char *buf);
// 申请内存（以页为单位）
void *user_malloc(uint32_t cnt);
// 释放内存（以页为单位）
void user_free(void *addr, uint32_t cnt);
// 用户版本的put_int
uint32_t print_int(uint32_t num);

#endif //HOS_SYSCALL_H
