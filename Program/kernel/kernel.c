#include "../lib/lib_kernel/lib_kernel.h"
#include "kernel_page/kernel_page.h"
#include "kernel_gdt/kernel_gdt.h"
#include "kernel_idt/kernel_idt.h"
#include "kernel_device/kernel_device.h"
#include "kernel_memory/kernel_memory.h"
#include "kernel_task/kernel_task.h"

/* 两个都是在线程中运行的函数 */
void k_thread_a(void* arg);
void k_thread_b(void* arg);

void kernel_main(void) {

    uint32_t total_physical_memory = *((uint32_t *)(0xa09));
    // 内存分页初始化
    init_paging();
    // 重新加载gdt
    setup_gdt();
    // 调整ESP，将栈指针切换到高地址（栈大小增加到0x1500）
    switch_esp_virtual_addr();
    // 初始化idt
    init_idt();
    // 初始化内存管理
    init_memory(total_physical_memory);
    // 初始化所有设备
    init_all_devices();
    // 初始化线程相关结构
    init_multitasking();

    task_create("k_thread_a", 31, k_thread_a, "argA ");
    task_create("k_thread_b", 8, k_thread_b, "argB ");

    // 允许PIC_IRQ0中断，才可以让定时器调度线程
    enable_interrupt(0);
    // 开启全局中断
    intr_enable();

    // 进入内核主循环或其它初始化代码
    for(;;) {
        put_str("MAIN");
        for(uint32_t i = 0; i < UINT16_MAX; i++);
    }
    // 退出主循环卸载设备驱动
    exit_all_devices();
}

/* 在线程中运行的函数 */
void k_thread_a(void* arg) {
/* 用void*来通用表示参数,被调用的函数知道自己需要什么类型的参数,自己转换再用 */
    char* para = arg;
    while(1) {
        put_str(para);
        for(uint32_t i = 0; i < UINT16_MAX; i++);
    }
}

/* 在线程中运行的函数 */
void k_thread_b(void* arg) {
/* 用void*来通用表示参数,被调用的函数知道自己需要什么类型的参数,自己转换再用 */
    char* para = arg;
    while(1) {
        put_str(para);
        for(uint32_t i = 0; i < UINT16_MAX; i++);
    }
}
