.section .text
.global switch_to

// 按照arm上C的调用约定
// r0是cur_task
// r1是next_task

switch_to:
    // 先前是在irq模式中运行task_switch函数的
    // 由于每次进入irq模式都统一栈地址，所以也不需要清理了
    // 每个任务中断时的信息都必须保存在其内核栈中
    cps     #0x13               // 切换到管理模式，使用当前任务的内核栈

    // 之前IRQ栈中有任务发生时状态的相关信息，直接将其复制过来然后压栈到内核栈
    // 经过先前的执行，寄存器的值可能和任务中断时的不同了
    // 所以才要考虑从IRQ栈里面取，而不是直接push

    // 统一使用高端映射（如果是进程的话，用的页表里面肯定没有内核的低端部分）
    // 统一用地址，出错好检查

    ldr     r4, =0xc0007fc4     // lr寄存器
    ldr     r5, [r4]
    push    {r5}

    ldr     r4, =0xc0007fc8     // 中断前的cpsr寄存器
    ldr     r5, [r4]
    push    {r5}

    ldr     r4, =0xc0007ffc     // r12寄存器
    ldr     r5, [r4]
    push    {r5}

    ldr     r4, =0xc0007ff8     // r11寄存器
    ldr     r5, [r4]
    push    {r5}

    ldr     r4, =0xc0007ff4     // r10寄存器
    ldr     r5, [r4]
    push    {r5}

    ldr     r4, =0xc0007ff0     // r9寄存器
    ldr     r5, [r4]
    push    {r5}

    ldr     r4, =0xc0007fec     // r8寄存器
    ldr     r5, [r4]
    push    {r5}

    ldr     r4, =0xc0007fe8     // r7寄存器
    ldr     r5, [r4]
    push    {r5}

    ldr     r4, =0xc0007fe4     // r6寄存器
    ldr     r5, [r4]
    push    {r5}

    ldr     r4, =0xc0007fe0     // r5寄存器
    ldr     r5, [r4]
    push    {r5}

    ldr     r4, =0xc0007fdc     // r4寄存器
    ldr     r5, [r4]
    push    {r5}

    ldr     r4, =0xc0007fd8     // r3寄存器
    ldr     r5, [r4]
    push    {r5}

    ldr     r4, =0xc0007fd4     // r2寄存器
    ldr     r5, [r4]
    push    {r5}

    ldr     r4, =0xc0007fd0     // r1寄存器
    ldr     r5, [r4]
    push    {r5}

    ldr     r4, =0xc0007fcc     // r0寄存器
    ldr     r5, [r4]
    push    {r5}

    // 保存任务环境之后就该更新栈指针（task结构体的self_stack字段，栈顶指针）
    str sp, [r0]                // str是将数据存储到内存地址，mov是寄存器之间传递数据

// ------------------  以上是备份当前任务的环境，下面是恢复下一个任务的环境  ----------------

    ldr sp, [r1]                // ldr是将数据从内存地址加载到寄存器

    pop     {r0-r9}             // 恢复这10个通用寄存器，剩下的三个有用

    // 用内核的高端映射部分，这部分是所有进程（包括内核）共享的资源，肯定都能用

    ldr     r11, =0xc0008000    // 保存r10寄存器
    pop     {r12}
    str     r12, [r11]

    ldr     r11, =0xc0008004    // 保存r11寄存器
    pop     {r12}
    str     r12, [r11]

    ldr     r11, =0xc0008008    // 保存r12寄存器
    pop     {r12}
    str     r12, [r11]

    pop     {r10}               // cpsr

    ldr     r11, =0xc000800c    // 保存lr寄存器
    pop     {r12}
    str     r12, [r11]

    ldr     r11, =0xc0008010    // 保存sp寄存器
    str     sp, [r11]

    // sp和lr都是模式变动就会失效，每个模式都有自己独立的sp和lr，只有通用寄存器不会失效
    // 所以必须重设cpsr之后，才能恢复sp和lr
    msr     cpsr_cxsf, r10       // 恢复下一个任务的cpsr

    // 恢复sp和lr
    ldr     r11, =0xc000800c    // 恢复lr寄存器
    ldr     r12, [r11]
    mov     lr, r12

    ldr     r11, =0xc0008010    // 恢复sp寄存器（目前想法是进程一样用内核栈，不再单独开设用户进程栈了，所以这么做）
    ldr     r12, [r11]
    mov     sp, r12

    ldr     r11, =0xc0008000    // 恢复r10寄存器
    ldr     r10, [r11]

    ldr     r11, =0xc0008004    // 恢复r11寄存器
    ldr     r11, [r11]

    ldr     r12, =0xc0008008    // 恢复r12寄存器
    ldr     r12, [r12]

    subs    pc, lr, #4          // 返回中断前的位置
