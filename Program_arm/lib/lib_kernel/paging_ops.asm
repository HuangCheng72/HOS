.section .text

.global enable_paging
.global load_page_directory
.global get_current_page

// void enable_paging();
enable_paging:
    mrc p15, 0, r0, c1, c0, 0   // 读取System Control Register (SCTLR)
    orr r0, r0, #0x1            // 设置第0位（M位）启用分页
    mcr p15, 0, r0, c1, c0, 0   // 写回SCTLR
    bx lr

// void load_page_directory(uint32_t page_directory_addr);
load_page_directory:
    // 从栈获取参数
    push {r4, lr}                // 保存寄存器
    ldr r0, [sp, #8]             // 获取第一个参数
    mcr p15, 0, r0, c2, c0, 0    // 设置页表基地址寄存器（TTBR0）
    pop {r4, lr}                 // 恢复寄存器
    bx lr

// uint32_t get_current_page(void);
get_current_page:
    mrc p15, 0, r0, c2, c0, 0    // 读取TTBR0寄存器
    bx lr
