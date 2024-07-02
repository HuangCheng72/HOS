//
// Created by huangcheng on 2024/6/24.
//

#include "../lib/lib_kernel/lib_kernel.h"
#include "kernel_page/kernel_page.h"

void kernel_main(void) {
    // 切换栈指针
    switch_sp(0x40007000);

    // 测试分页后能不能用
    *((uint32_t *)0x40004000) = 0x1234;
    put_int(*((uint32_t *)0x40004000));
    put_char('\n');

    // 页表初始化
    init_paging();

    // 查看是否分页成功
    put_int(*((uint32_t *)0xc0004000));
    put_char('\n');

    for(;;);
}
