#include "../lib/lib_kernel/lib_kernel.h"
#include "kernel_page/kernel_page.h"
#include "kernel_gdt/kernel_gdt.h"
#include "kernel_idt/kernel_idt.h"
#include "kernel_device/kernel_device.h"
#include "kernel_memory/kernel_memory.h"
#include "kernel_task/kernel_task.h"
#include "../devices/console/console.h"
#include "kernel_buffer//kernel_buffer.h"

// 键盘监视任务（实际上监视对象是分配给键盘的缓冲区）
void keyboard_task();

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
    // 初始化线程相关结构
    init_multitasking();
    // 初始化所有设备
    init_all_devices();


    // 开一个键盘监视任务，不然就要用内核主任务监视了
    task_create("keyboard_task", 31, keyboard_task, NULL);

    // 允许PIC_IRQ0中断，才可以让定时器调度线程
    enable_interrupt(0);
    // 允许PIC_IRQ1中断，才可以处理键盘输入
    enable_interrupt(1);
    // 开启全局中断
    intr_enable();

    // 进入内核主循环或其它初始化代码
    for(;;) {

    }
    // 退出主循环卸载设备驱动
    exit_all_devices();
}

// 键盘监视任务（实际上监视对象是分配给键盘的缓冲区）
void keyboard_task() {
    struct driver *keyboard_driver = get_driver("keyboard");
    char data;
    for(;;) {
        if(device_read(keyboard_driver, &data, 1) > 0) {
            // 有数据就全部打到屏幕上
            console_put_char(data);
        }
    }
}
