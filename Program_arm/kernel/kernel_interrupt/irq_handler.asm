.section .text
.global irq_handler
.extern irq_interrupt_dispatcher

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
    // 如果图方便的话就在此时进入管理模式，这样就可以用管理模式下的栈了
    // 一般来说还是推荐分离，分离的话在初始化的时候使用
    // 如果用管理模式栈的话，在这里切换管理模式即可
    // 但是那样就没法获取spsr中的值了

    push    {r0-r12, lr}    // 由于我担心可能用到的寄存器多，所以我保存了所有的通用寄存器以及lr（不然无法返回）

    mrs     r0, spsr        // 保存中断前的cpsr
    push    {r0}

    cps     #0x13           // 不管什么模式，都要切换到管理模式以处理中断（切换到内核态）

    // C语言是压栈传参的，但是这里没参数
    bl irq_interrupt_dispatcher // 调用IRQ中断分发器，bl指令会自动保存lr，能回得来

    cps     #0x12           // 切换回IRQ模式，要不然就无法获取这些值了

    pop     {r0}            // 恢复cpsr的时候也恢复了原来的模式
    msr     spsr_cxsf, r0

    // _cxsf后缀意义如下：
    // c（Condition flags）：
    // 条件标志，包括N（负）、Z（零）、C（进位/溢出）和V（溢出）。
    // 这些标志位用于条件执行指令的判断，例如比较和分支。
    // x（Extension flags）：
    // 扩展标志，包含J（Jazelle状态）和GE[3:0]（Greater than or Equal flags）。
    // 这些标志位在一些特定的处理器模式和扩展中使用，例如Jazelle模式和高级SIMD（NEON）操作。
    // s（State flags）：
    // 状态标志，包括T（Thumb状态）、E（字节序）和IT[7:0]（If-Then状态）。
    // 这些标志位用于指示处理器当前的执行状态，如是Thumb指令集还是ARM指令集，或者当前的字节序设置。
    // f（Flags field）：
    // 标志字段，包括中断屏蔽标志I（IRQ禁用）、F（FIQ禁用）和A（异步中止禁用）。
    // 这些标志位用于控制中断的使能和屏蔽状态。
    // 加上这个标志表示全部修改，不加的话等效于spsr_c，只能修改条件标志

    pop     {r0-r12, lr}    // 恢复保存的寄存器
    cpsie i                 // 启用IRQ中断
    subs    pc, lr, #4      // 返回中断前的位置（CPU会自动恢复spsr_irq到cpsr）
