#include "../lib/lib_kernel/lib_kernel.h"
#include "kernel_page/kernel_page.h"
#include "kernel_gdt/kernel_gdt.h"
#include "kernel_idt/kernel_idt.h"
#include "kernel_device/kernel_device.h"
#include "kernel_memory/kernel_memory.h"

void kernel_main(void) {

    // 内存分页初始化
    init_paging();
    // 重新加载gdt
    setup_gdt();
    // 调整ESP，将栈指针切换到高地址
    switch_esp_virtual_addr() ;
    // 初始化idt
    init_idt();
    // 初始化内核必需的设备
    init_device();
    // 初始化内存管理
    init_memory();



    // 进入内核主循环或其它初始化代码
    for(;;) {

    }
}
