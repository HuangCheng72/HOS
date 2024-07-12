.section .text
.global irq_handler
.extern irq_interrupt_dispatcher
.extern task_switch

// 关于arm的模式
// 0b10000 或 0x10：用户模式（User Mode）
// 0b10001 或 0x11：快速中断模式（FIQ Mode）
// 0b10010 或 0x12：中断模式（IRQ Mode）
// 0b10011 或 0x13：管理模式（Supervisor Mode）
// 0b10111 或 0x17：中止模式（Abort Mode）
// 0b11011 或 0x1B：未定义模式（Undefined Mode）
// 0b11111 或 0x1F：系统模式（System Mode，ARMv4以上）

irq_handler:
    // 进入的时候已经为IRQ模式
    // 进入该模式的时候，自动保存中断前cpsr到spsr寄存器（准确来说是spsr_irq，这是irq模式下才能读写的）
    // 此时lr是中断前pc + 4（下一条指令地址，CPU自动保存）
    cpsid i                 // 禁用IRQ中断
    // irq模式有irq模式自己的栈（u-boot设置过了，栈底直接在0xffffffff处）

    // 从arm v6开始，arm提供了两条汇编指令，专门用于中断上下文保存
    // srsdb，直接把spsr和lr保存到指定位置，还可以切换到其他模式，一般是srsdb sp!, #0x13（保存到当前模式栈，然后切换到管理模式）
    // rfeia，从指定位置恢复spsr和lr，并且直接跳转回lr指向的指令，直接恢复中断处理前的模式，一般是rfeia sp!
    // srsdb压栈顺序是先spsr，后lr，rfeia弹栈顺序相反，这两个函数不需要寄存器中继

    // 统一IRQ栈地址，这样就不用考虑清理IRQ栈的问题了
    ldr     sp, =0x40008000

    // 直接保存所有通用寄存器到IRQ栈（中断发生时所有上下文都被保存了）
    push    {r0-r12}

    // IRQ模式的栈和管理模式的栈分离，完全没办法用x86下的做法了
    // 我又一时间发明不了新的做法
    // 只能尝试约定一个位置专门存放这两个元素，直接放在IRQ栈，位置固定，好解决

    // 从arm v2开始，arm支持stmdb和ldmia指令，这两个指令可以直接把寄存器列表保存到指定地方
    // push和pop相当于限定死了目的地是栈，这两个指令比较自由，但是这两个指令好像不能保存spsr
    // 为了可读性还是用push和pop吧，这两个一看就知道

    mrs     r0, spsr            // 保存中断前的cpsr
    push    {r0}                // 压栈保存spsr
    push    {lr}                // 压栈保存lr

    // IRQ模式也算是特权模式，可以视为内核态，反正也能处理就是了

    bl irq_interrupt_dispatcher // 调用IRQ中断分发器，bl指令会自动保存lr，能回得来

    // 尝试切换任务
    // 这时候尝试切换，如果回得来，不怕数据丢失
    // 如果回不来，重入中断的时候栈指针又是重设的，不怕栈没清空
    // 至于CPSR，状态问题更不用担心了，哪个进入中断之前都是允许中断的
    bl      task_switch

    pop     {lr}                // 恢复lr
    pop     {r0}                // 恢复spsr
    msr     spsr_cxsf, r0

    pop     {r0-r12}            // 恢复其他上下文


    cpsie i                     // 启用IRQ中断
    subs    pc, lr, #4          // 返回中断前的位置（CPU会自动恢复spsr_irq到cpsr）
