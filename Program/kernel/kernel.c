#include "../lib/lib.h"
#include "kernel_page/kernel_page.h"
#include "kernel_gdt/kernel_gdt.h"

extern void print_char(char c, uint16_t pos);
extern void io_hlt();

void kernel_main(void) {
    print_char('K', 160);

    // 内存分页初始化
    init_paging();
    // 重新加载gdt
    setup_gdt();
    // 调整ESP，将栈指针切换到高地址
    switch_esp_virtual_addr() ;

    print_char('I', 160);

    // 进入内核主循环或其它初始化代码
    for(;;) {
        io_hlt();
    }
}
