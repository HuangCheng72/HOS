//
// Created by huangcheng on 2024/6/3.
//

#include "console.h"
#include "../../lib/lib_kernel/lib_kernel.h"
#include "../../kernel/kernel_task/kernel_sync.h"
#include "../../kernel/kernel_device/kernel_device.h"

// 终端锁是用递归锁（作用域限制在终端范围内，其他地方不得使用，所以加上static）
static struct recursive_mutex console_lock;

// 初始化唯一终端（控制台）
void console_init() {
    recursive_mutex_init(&console_lock);
}

// 获取终端，进入临界区
void console_acquire() {
    recursive_mutex_lock(&console_lock);
}

// 释放终端，退出临界区
void console_release() {
    recursive_mutex_unlock(&console_lock);
}

// 终端打印字符串
void console_put_str(const char* str) {
    console_acquire();
    put_str(str);
    console_release();
}

// 终端打印字符
void console_put_char(char char_asci) {
    console_acquire();
    put_char(char_asci);
    console_release();
}

// 终端打印16进制整数
void console_put_int(int num) {
    console_acquire();
    put_int(num);
    console_release();
}

// 注册宏，定义一个驱动结构体，把驱动结构体实例放到驱动段
REGISTER_DRIVER(console_driver){
        .driver_name = "console",
        .init = console_init,
        .exit = NULL,
        .irq = -1,
        .irq_interrupt_handler = NULL,
        .need_kernel_buffer = 0    // 明确不需要缓冲区，其实不写也行，初始值就是0
};
