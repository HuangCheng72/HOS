//
// Created by huangcheng on 2024/6/9.
//

#include "syscall.h"

// 汇编写的发出中断开启系统调用函数
extern uint32_t intr_syscall(uint32_t syscall_number, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);

// 用户版本的put_str
uint32_t write(char* str) {
    return intr_syscall(1,(uint32_t)str,0,0,0);
}

void get_process_name(char *buf) {
    intr_syscall(2,(uint32_t)buf,0,0,0);
}

// 申请内存（以页为单位）
void *user_malloc(uint32_t cnt) {
    return (void *)intr_syscall(3,cnt,0,0,0);
}

// 释放内存（以页为单位）
void user_free(void *addr, uint32_t cnt) {
    intr_syscall(4,(uint32_t)addr,cnt,0,0);
}
