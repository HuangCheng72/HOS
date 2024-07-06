.section .text
.global disable_interrupts
.global enable_interrupts

// void disable_interrupts();
disable_interrupts:
    cpsid i                 // 禁用中断
    bx lr                   // 返回调用者


// void enable_interrupts();
enable_interrupts:
    cpsie i                 // 启用中断
    bx lr                   // 返回调用者
