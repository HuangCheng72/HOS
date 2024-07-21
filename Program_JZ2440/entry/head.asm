.section .text.startup

.global _start
.extern disable_watch_dog
.extern memsetup
.extern disable_watch_dog
.extern kernel_main

_start:
// 0x00：直接重启
    b   Reset

// 0x04：未定义异常处理
HandleUndef:
    b   HandleUndef

// 0x08：软件异常（用SWI指令打出来的）处理
HandleSWI:
    b   HandleSWI

// 0x0c: 指令预取终止导致的异常处理
HandlePrefetchAbort:
    b   HandlePrefetchAbort

// 0x10: 数据访问终止导致的异常处理
HandleDataAbort:
    b   HandleDataAbort

// 0x14: 保留，不使用
HandleNotUsed:
    b   HandleNotUsed

// 0x18: 中断异常处理
HandleIRQ:
    b   HandleIRQ

// 0x1c: 快中断模式的向量地址
HandleFIQ:
    b   HandleFIQ

Reset:
    ldr sp, =0x1000                     // 设置栈指针，调用C函数之前要设置栈，才有C语言运行环境
    bl  disable_watch_dog               // 关看门狗
    bl  memsetup                        // 设置存储控制器以使用SDRAM
    bl  copy_steppingstone_to_sdram     // 复制这部分资源代码到SDRAM中
    ldr pc, =on_sdram                   // 跳到SDRAM中继续执行
on_sdram:
    ldr sp, =0x30007000                 // 设置新栈
    bl  kernel_main
halt_loop:
    b   halt_loop
