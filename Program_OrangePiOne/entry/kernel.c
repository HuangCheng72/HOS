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

void led_blink_test(void *args);

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

    task_create("led_blink_test", 31, led_blink_test, NULL);

    // 开启IRQ中断
    intr_enable();
    // 允许定时器中断
    enable_gic_irq_interrupt(50);

    for(;;) {
        // 没什么事就让CPU休息
        task_idle();
    }
}

void led_blink_test(void *args) {
    // 获取GPIO驱动指针
    struct driver *gpio_driver = get_driver("gpio");
    if (gpio_driver == NULL) {
        // 处理未找到驱动的错误
        return;
    }

    // 参数结构体
    struct gpio_request {
        char group;     // 组号，如 'A', 'B', 'C', ... 'L'
        uint8_t pin;    // 针脚编号（0-31）
        uint8_t value;  // 用于写操作的电平值，0表示低电平，1表示高电平
    };

    struct gpio_request green_led_on = {'L',10, 1};     // GPIO L10 绿色LED 开
    struct gpio_request green_led_off = {'L',10, 0};    // GPIO L10 绿色LED 关
    struct gpio_request red_led_on = {'A',15, 1};       // GPIO A15 红色LED 开
    struct gpio_request red_led_off = {'A',15, 0};      // GPIO A15 红色LED 关

    for(;;) {
        // 打开绿色LED，关闭红色LED
        device_write(gpio_driver, (char *)&green_led_on, sizeof(green_led_on));
        device_write(gpio_driver, (char *)&red_led_off, sizeof(red_led_off));
        for(uint32_t i = 0; i < 8 * UINT16_MAX; i++); // 延时一段时间

        // 关闭绿色LED，打开红色LED
        device_write(gpio_driver, (char *)&green_led_off, sizeof(green_led_off));
        device_write(gpio_driver, (char *)&red_led_on, sizeof(red_led_on));
        for(uint32_t i = 0; i < 8 * UINT16_MAX; i++); // 延时一段时间
    }
}
