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

    ldr     r4, =0x40007ffc     // r12寄存器
    ldr     r5, [r4]
    push    {r5}

    sub     r4, r4, #4          // r11寄存器
    ldr     r5, [r4]
    push    {r5}

    sub     r4, r4, #4          // r10寄存器
    ldr     r5, [r4]
    push    {r5}

    sub     r4, r4, #4          // r9寄存器
    ldr     r5, [r4]
    push    {r5}

    sub     r4, r4, #4          // r8寄存器
    ldr     r5, [r4]
    push    {r5}

    sub     r4, r4, #4          // r7寄存器
    ldr     r5, [r4]
    push    {r5}

    sub     r4, r4, #4          // r6寄存器
    ldr     r5, [r4]
    push    {r5}

    sub     r4, r4, #4          // r5寄存器
    ldr     r5, [r4]
    push    {r5}

    sub     r4, r4, #4          // r4寄存器
    ldr     r5, [r4]
    push    {r5}

    sub     r4, r4, #4          // r3寄存器
    ldr     r5, [r4]
    push    {r5}

    sub     r4, r4, #4          // r2寄存器
    ldr     r5, [r4]
    push    {r5}

    sub     r4, r4, #4          // r1寄存器
    ldr     r5, [r4]
    push    {r5}

    sub     r4, r4, #4          // r0寄存器
    ldr     r5, [r4]
    push    {r5}

    sub     r4, r4, #4          // cpsr寄存器
    ldr     r5, [r4]
    push    {r5}

    sub     r4, r4, #4          // lr寄存器
    ldr     r5, [r4]
    push    {r5}

    // 保存任务环境之后就该更新栈指针（task结构体的self_stack字段，栈顶指针）
    str sp, [r0]                // str是将数据存储到内存地址，mov是寄存器之间传递数据

// ------------------  以上是备份当前任务的环境，下面是恢复下一个任务的环境  ----------------

    ldr sp, [r1]                // ldr是将数据从内存地址加载到寄存器

    // 然后就是恢复数据了
    pop     {lr}                // 恢复lr
    pop     {r4}                // 把下一个任务的cpsr弹出到r4，然后恢复到cpsr
    msr     cpsr_cxsf, r4       // 恢复下一个任务的cpsr
    pop     {r0-r12}            // 恢复这13个寄存器

    subs    pc, lr, #4          // 返回中断前的位置（CPU会自动恢复spsr_irq到cpsr）
