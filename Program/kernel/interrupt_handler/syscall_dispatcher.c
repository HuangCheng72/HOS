//
// Created by huangcheng on 2024/6/8.
//

// 可能要用到内核的全部功能，所以都要include
#include "../kernel_task/kernel_task.h"
#include "../kernel_task/kernel_sync.h"
#include "../kernel_memory/kernel_memory.h"
#include "../kernel_device/kernel_device.h"
#include "../kernel_buffer/kernel_buffer.h"
#include "../../devices/console/console.h"

// 理论上这些专属于内核，用户进程就不该能用
#include "../kernel_page/kernel_page.h"
#include "../kernel_gdt/kernel_gdt.h"
#include "../kernel_idt/kernel_idt.h"
#include "../kernel_interrupt/kernel_interrupt.h"
#include "../../devices/keyboard/keyboard.h"
#include "../../devices/pit8253/pit8253.h"


// 系统调用分发器
uint32_t syscall_dispatcher(uint32_t syscall_number, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {

    // 根据约定的调用号来调用各个功能
    if(syscall_number == 0) {
        // 这是为了防止默认值，所以0设为无效
        return 0;
    }

    if(syscall_number == 1) {
        console_put_str((char *)arg1);
        return 1;
    }

    if(syscall_number == 2) {
        // 不管溢出，这是用户考虑的问题
        strcpy((char *)arg1, running_task()->name);
        return 1;
    }

    if(syscall_number == 3) {
        return (uint32_t)malloc_page(USER_FLAG, arg1);
    }

    if(syscall_number == 4) {
        free_page((void *)arg1, arg2);
    }

    return 0;
}