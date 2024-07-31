//
// Created by huangcheng on 2024/6/24.
//

#include "../lib/lib_kernel/lib_kernel.h"
#include "../kernel/kernel_page/kernel_page.h"
#include "../kernel/kernel_task/kernel_task.h"
#include "../kernel/kernel_memory/kernel_memory.h"
#include "../kernel/kernel_interrupt/kernel_interrupt.h"
#include "../kernel/kernel_device/kernel_device.h"

#include "../devices/console/console.h"

void task_test(void *args);
void task_test2(void *args);

void kernel_main(void) {
    // 页表初始化
    // 实测，由于u-boot的页表设置了扩展地址和很多东西（缓存缓冲等）
    // 导致必须关掉MMU之后才能进行页表的初始化（要不然直接写不进真正的物理内存）
    // 排查那么多天排除出这么个结果我也是没话说了
    // 我非常想把禁用MMU和启用MMU写入init_paging中，但是这样的话直接跳不回来，会直接进入数据错误异常处理程序
    // 果然虚拟机还是不靠谱

    // 先禁用MMU
    // 获取当前SCTLR寄存器值
    SCTLR_t sctlr;
    get_sctlr(&sctlr);
    // 禁用MMU
    sctlr.M = 0; // 禁用MMU
    sctlr.A = 0; // 禁用对齐检查
    sctlr.C = 0; // 禁用数据缓存
    // 更新SCTLR寄存器
    set_sctlr(&sctlr);

    // 现在开始才能初始化页表
    init_paging();

    // 启用MMU
    get_sctlr(&sctlr);
    sctlr.M = 1; // 启用MMU
    sctlr.A = 1; // 启用对齐检查
    sctlr.C = 1; // 启用数据缓存
    // 更新SCTLR寄存器
    set_sctlr(&sctlr);

    // 加载内核栈
    switch_sp(0xc0007000);
    // 初始化task
    init_multitasking();
    // 初始化内存管理（本来这里应该从环境里面获取内存大小，我懒，直接写死了算了）
    init_memory(0x20000000);
    // 初始化中断管理和GIC
    init_interrupt();
    // 初始化设备驱动管理
    init_all_devices();

    // 这里需要清除一下0xc0008080处的值
    // 搞不明白这里哪来的值，难道是上电初始化不为0？还是u-boot把这地方写了？
    *((uint32_t *)0xc0008080) = 0;

    task_create("task_test", 31, task_test, NULL);
    task_create("task_test2", 31, task_test2, NULL);

    // 开启IRQ中断
    intr_enable();
    // 允许定时器中断
    enable_gic_irq_interrupt(50);

    for(;;);
}

void task_test(void *args) {
    uint32_t counter = 0;
    for (;;) {
        counter++;
        console_printf("counter1 : %d\n", counter);
        for(uint32_t i = 0; i < 16 * UINT16_MAX; i++);
    }
}

void task_test2(void *args) {
    uint32_t counter = 0;
    for (;;) {
        counter++;
        console_printf("counter2 : %d\n", counter);
        for(uint32_t i = 0; i < 16 * UINT16_MAX; i++);
    }
}
