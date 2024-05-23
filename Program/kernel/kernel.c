#include "../lib/lib.h"
#include "kernel_page/kernel_page.h"
#include "kernel_gdt/kernel_gdt.h"

extern void print_char(char c, uint16_t pos);
extern void io_hlt();

void kernel_main(void) {
    print_char('K', 160);

    // 内存分页初始化
    init_paging();

    // 进入内核主循环或其它初始化代码
    for(;;) {
        io_hlt();
    }
}
