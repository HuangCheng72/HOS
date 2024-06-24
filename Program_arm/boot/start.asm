.section .text
.global _start

_start:
    // 设置栈指针
    ldr sp, =stack_top

    // 跳转到内核主函数
    bl kernel_main

hang:
    // 无限循环，防止返回
    b hang

// 定义堆栈
.section .bss
.align 12
stack_top:
    .space 0x6000  // 分配24KB的内核栈
