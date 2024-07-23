//
// Created by huangcheng on 2024/6/24.
//

#include "../lib/lib_kernel/lib_kernel.h"
#include "kernel_page/kernel_page.h"
#include "kernel_task/kernel_task.h"
#include "kernel_interrupt/kernel_interrupt.h"
#include "kernel_device/kernel_device.h"
#include "../devices/console/console.h"

/*GPIO registers*/
#define GPBCON              (*(volatile unsigned long *)0x56000010)
#define GPBDAT              (*(volatile unsigned long *)0x56000014)

#define GPFCON              (*(volatile unsigned long *)0x56000050)
#define GPFDAT              (*(volatile unsigned long *)0x56000054)
#define GPFUP               (*(volatile unsigned long *)0x56000058)

#define GPGCON              (*(volatile unsigned long *)0x56000060)
#define GPGDAT              (*(volatile unsigned long *)0x56000064)
#define GPGUP               (*(volatile unsigned long *)0x56000068)

#define GPHCON              (*(volatile unsigned long *)0x56000070)
#define GPHDAT              (*(volatile unsigned long *)0x56000074)
#define GPHUP               (*(volatile unsigned long *)0x56000078)

/*UART registers*/
#define ULCON0              (*(volatile unsigned long *)0x50000000)
#define UCON0               (*(volatile unsigned long *)0x50000004)
#define UFCON0              (*(volatile unsigned long *)0x50000008)
#define UMCON0              (*(volatile unsigned long *)0x5000000c)
#define UTRSTAT0            (*(volatile unsigned long *)0x50000010)
#define UTXH0               (*(volatile unsigned char *)0x50000020)
#define URXH0               (*(volatile unsigned char *)0x50000024)
#define UBRDIV0             (*(volatile unsigned long *)0x50000028)

#define TXD0READY   (1<<2)
#define RXD0READY   (1)

#define PCLK            50000000    // 设置时钟频率
#define UART_CLK        PCLK        // UART0的时钟源设为PCLK
#define UART_BAUD_RATE  115200      // 波特率
#define UART_BRD        ((UART_CLK  / (UART_BAUD_RATE * 16)) - 1)

/*
 * 初始化UART0
 * 115200,8N1,无流控
 */
void uart0_init(void) {
    GPHCON  |= 0xa0;    // GPH2,GPH3用作TXD0,RXD0
    GPHUP   = 0x0c;     // GPH2,GPH3内部上拉

    ULCON0  = 0x03;     // 8N1(8个数据位，无较验，1个停止位)
    UCON0   = 0x05;     // 查询方式，UART时钟源为PCLK
    UFCON0  = 0x00;     // 不使用FIFO
    UMCON0  = 0x00;     // 不使用流控
    UBRDIV0 = UART_BRD; // 波特率为115200
}

void task_test(void *args) {
    uint32_t counter = 0;
    for (;;) {
        counter++;
        console_printf("counter1 : %d\n", counter);
        for(uint32_t i = 0; i < 4 * UINT16_MAX; i++);
    }
}

void task_test2(void *args) {
    uint32_t counter = 0;
    for (;;) {
        counter++;
        console_printf("counter2 : %d\n", counter);
        for(uint32_t i = 0; i < 4 * UINT16_MAX; i++);
    }
}


void kernel_main(void) {
    uart0_init();   // 波特率115200，8N1(8个数据位，无校验位，1个停止位)
    // 内存分页初始化
    init_paging();
    // 重新加载内核栈
    switch_sp(0xc0007000);
    // 初始化task
    init_multitasking();
    // 初始化内存管理（JZ2440的内存大小是64MB，也就是0x4000000，可以直接写死）
    init_memory(0x4000000);
    // 初始化中断管理和GIC
    init_interrupt();
    // 初始化设备驱动管理
    init_all_devices();

    // 创建一个任务，这个任务主要的作用是让CPU休息，进入低功耗状态
    task_create("task_idle", 31, task_idle, NULL);

    task_create("task_test", 16, task_test, NULL);
    task_create("task_test2", 16, task_test2, NULL);

    enable_gic_irq_interrupt(10);
    // 开启IRQ中断
    intr_enable();

    for(;;) {
        // 内核没什么事就尽量让出CPU时间给其他任务，可不敢让内核wfi
        task_yield();
    }

    // 以防万一，退出时退出所有设备
    exit_all_devices();
}
