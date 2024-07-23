//
// Created by huangcheng on 2024/6/3.
//

#include "console.h"
#include "../../lib/lib_kernel/lib_kernel.h"
#include "../../kernel/kernel_task/kernel_sync.h"
#include "../../kernel/kernel_device/kernel_device.h"

// 终端锁是用递归锁（作用域限制在终端范围内，其他地方不得使用，所以加上static）
static struct recursive_mutex console_lock;

// 注册宏，定义一个驱动结构体，把驱动结构体实例放到驱动段
REGISTER_DRIVER(console_driver){
        .driver_name = "console",
        .init = console_init,
        .exit = NULL,
        .irq = -1,
        .irq_interrupt_handler = NULL,
        .need_data_buffer = 1,
        .data_buffer = NULL
};

// 初始化唯一终端（控制台）
void console_init() {
    // 缓冲区检查
    if(!console_driver.data_buffer) {
        put_str("\n--------- CONSOLE_INIT_ERROR! ----------\n");
        for(;;);
    }
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

// 终端打印格式化字符串
void console_printf(const char* format, ...) {
    console_acquire();

    struct kernel_buffer* buffer = (struct kernel_buffer*)console_driver.data_buffer;
    char temp_buffer[256];
    const char *p;
    int int_temp;
    unsigned int uint_temp;
    char char_temp;
    char *str_temp;
    void* ptr_temp;
//    double double_temp;
    // 讽刺，出现double类型都不行，我甚至都没运算

    // 获取可变参数列表的起始地址
    char* arg_ptr = (char*)(&format + 1);

    for(p = format; *p != '\0'; p++) {
        if(*p != '%') {
            kernel_buffer_write(buffer, (char *)p, 1);
            continue;
        }
        p++;
        int width = 0;
        int precision = -1;

        // 读取宽度
        while (*p >= '0' && *p <= '9') {
            width = width * 10 + (*p - '0');
            p++;
        }

        // 读取精度
        if (*p == '.') {
            p++;
            precision = 0;
            while (*p >= '0' && *p <= '9') {
                precision = precision * 10 + (*p - '0');
                p++;
            }
        }

        switch(*p) {
            case 'd':
                int_temp = *((int*)arg_ptr);
                arg_ptr += sizeof(int);
                int_to_str(int_temp, temp_buffer);
                break;
            case 'u':
                uint_temp = *((unsigned int*)arg_ptr);
                arg_ptr += sizeof(unsigned int);
                uint_to_str(uint_temp, temp_buffer);
                break;
            case 'x':
                uint_temp = *((unsigned int*)arg_ptr);
                arg_ptr += sizeof(unsigned int);
                hex_to_str(uint_temp, temp_buffer);
                break;
            case 'p':
                ptr_temp = *((void**)arg_ptr);
                arg_ptr += sizeof(void*);
                ptr_to_str(ptr_temp, temp_buffer);
                break;
//            case 'f':     // 这个真没办法，只能弃用，原因我在string里面有说
//                double_temp = *((double*)arg_ptr);
//                arg_ptr += sizeof(double);
//                double_to_str(double_temp, temp_buffer, precision == -1 ? 6 : precision); // 默认精度为6
//                break;
            case 's':
                str_temp = *((char**)arg_ptr);
                arg_ptr += sizeof(char*);
                kernel_buffer_write(buffer, str_temp, strlen(str_temp));
                continue;
            case 'c':
                char_temp = *((char*)arg_ptr);
                arg_ptr += sizeof(char);
                kernel_buffer_write(buffer, &char_temp, 1);
                continue;
            case '%':
                kernel_buffer_write(buffer, (char *)p, 1);
                continue;
            default:
                kernel_buffer_write(buffer, (char *)p, 1);
                continue;
        }

        int len = strlen(temp_buffer);
        int pad_len = width - len;
        if (pad_len > 0) {
            while (pad_len-- > 0) {
                kernel_buffer_write(buffer, " ", 1);
            }
        }
        kernel_buffer_write(buffer, temp_buffer, len);

        // 输出缓冲区中的内容
        while (!kernel_buffer_is_empty(buffer)) {
            char output_char;
            kernel_buffer_read(buffer, &output_char, 1);
            put_char(output_char);
        }
    }

    // 将剩余的缓冲区内容打印到控制台
    while (!kernel_buffer_is_empty(buffer)) {
        char output_char;
        kernel_buffer_read(buffer, &output_char, 1);
        put_char(output_char);
    }

    console_release();
}
