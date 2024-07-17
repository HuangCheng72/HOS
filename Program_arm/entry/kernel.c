//
// Created by huangcheng on 2024/6/24.
//

#include "../lib/lib_kernel/lib_kernel.h"
#include "../kernel/kernel_page/kernel_page.h"
#include "../kernel/kernel_task/kernel_task.h"
#include "../kernel/kernel_interrupt/kernel_interrupt.h"
#include "../kernel/kernel_device/kernel_device.h"
#include "../devices/console/console.h"
#include "../devices/rtc_cmos/rtc_cmos.h"

void kernel_main(void) {
    // u-boot已经给我们设置了管理模式
    // 一进来就已经是管理模式了

    // 页表初始化
    init_paging();

    // 因为覆盖了整个内存区域，所以还可以用u-boot的栈
    // 到这里再切换也没问题

    // 切换栈指针到虚拟地址，这样就可以顺利使用内核栈了
    switch_sp(0xc0007000);
    // 初始化task
    init_multitasking();
    // 初始化内存管理（本来这里应该从环境里面获取内存大小，我懒，直接写死了算了）
    init_memory(0x8000000);
    // 初始化中断管理和GIC
    init_interrupt();
    // 初始化设备驱动管理
    init_all_devices();

    // 创建一个任务，这个任务主要的作用是让CPU休息，进入低功耗状态
    task_create("task_idle", 31, task_idle, NULL);

    // 开启IRQ中断
    intr_enable();

    // 允许定时器中断
    enable_gic_irq_interrupt(30);

    for(;;) {
        // 内核没什么事就尽量让出CPU时间给其他任务，可不敢让内核wfi
        task_yield();
    }

    // 以防万一，退出时退出所有设备
    exit_all_devices();
}
