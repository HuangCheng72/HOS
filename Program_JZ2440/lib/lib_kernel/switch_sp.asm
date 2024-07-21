.section .text

.global switch_sp

// 切换栈指针
// void switch_sp(uint32_t new_sp);
switch_sp:
    // 第一个参数通过r0传参
    mov sp, r0
    bx lr
