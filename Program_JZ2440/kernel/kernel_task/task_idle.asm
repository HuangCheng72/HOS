.section .text
.global task_idle

task_idle:
    // 因为GCC老是报处理器不支持处理器模式
    // 干脆直接写汇编
    wfi
    b task_idle