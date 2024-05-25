#include "../lib/lib.h"
#include "kernel_page/kernel_page.h"
#include "kernel_gdt/kernel_gdt.h"
#include "kernel_idt/kernel_idt.h"

void kernel_main(void) {

    //put_str("This is a kernel.\n");
    //put_str("After the kernel initialization completes.\n");
    //put_str("The kernel will print integer '1024' on screen.\n");
    //put_str("'1024' in hexadecimal format is '0x400'.\n");

    // 内存分页初始化
    init_paging();
    // 重新加载gdt
    setup_gdt();
    // 调整ESP，将栈指针切换到高地址
    switch_esp_virtual_addr() ;
    // 初始化idt
    init_idt();

    put_int(1024);

    // 进入内核主循环或其它初始化代码
    for(;;) {

    }
}
