.section .text
.global task_idle

task_idle:
    // 因为GCC老是报处理器不支持处理器模式
    // 干脆直接写汇编
    mcr p15, 0, r0, c7, c0, 4   //arm v4T架构不支持直接的wfi指令，只能自己实现进入低功耗模式等待中断
    b task_idle