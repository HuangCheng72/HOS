//
// Created by huangcheng on 2024/6/1.
//

#ifndef HOS_KERNEL_THREAD_H
#define HOS_KERNEL_THREAD_H

#include "../../lib/lib_kernel/lib_kernel.h"

// 任务函数，等效于 void (*)(void*) 类型
typedef void task_function(void*);

// 最常用的五大任务状态（进线程通用），正在运行，准备就绪，阻塞，等待，挂起，死亡
enum task_status {
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_WAITING,
    TASK_HANGING,
    TASK_DIED
};

//PCB（程序控制块），表明这是一个任务（不管是进线程，都是任务）
struct task {
    uint8_t *self_stack;	    // 各任务都用自己的栈（指向栈顶）

    enum task_status status;    // 任务状态，这个不用说
    char name[16];              // 任务名（未来可以改成全局唯一ID）
    uint8_t priority;           // 优先级
    uint8_t ticks;	            // 每次在处理器上执行的时间嘀嗒数（一个ticks即定时器IRQ0中断一次，决定什么时候调度任务）

    uint32_t elapsed_ticks;     // 此任务自上cpu运行后至今占用了多少cpu嘀嗒数,

    struct list_node general_tag;   // 当前串在哪个调度队列上
    struct list_node all_task_tag;  // 所有任务都串在一起，这是为了保证任何时候都能找到

    uint32_t* pgdir;            // 页表的地址（分配这个资源的时候就是进程了）

    uint32_t stack_magic;	    // 用这串数字做栈的边界标记,用于检测任务栈的溢出
};

// 中断栈数据
struct interrupt_stack {
    uint32_t interrupt_number;  // 中断号
    uint32_t manual_error_code; // 手动压栈的错误码 (如果没有错误码，这里存储0)
    uint32_t gs;         // 附加段寄存器 GS
    uint32_t fs;         // 附加段寄存器 FS
    uint32_t es;         // 额外段寄存器 ES
    uint32_t ds;         // 数据段寄存器 DS
    uint32_t edi;        // 目标变址寄存器 EDI
    uint32_t esi;        // 源变址寄存器 ESI
    uint32_t ebp;        // 基址指针寄存器 EBP
    uint32_t esp_dummy;  // 栈指针寄存器 ESP（不会真的恢复到ESP，所以只是个摆设）
    uint32_t ebx;        // 通用寄存器 EBX
    uint32_t edx;        // 通用寄存器 EDX
    uint32_t ecx;        // 通用寄存器 ECX
    uint32_t eax;        // 通用寄存器 EAX
    uint32_t error_code; // 错误码 (如果没有错误码，这里存储0)
    uint32_t eflags;     // 标志寄存器
    uint32_t cs;         // 代码段寄存器
    uint32_t eip;        // 指令指针寄存器
    uint32_t user_esp;   // 低特权级进入高特权级的时候CPU自动压的栈指针 ESP
    uint32_t user_ss;    // 低特权级进入高特权级的时候CPU自动压的栈段选择子 SS
};


// 任务信息栈，用于保存一个任务切换时候的状态
struct task_info_stack {
    // 这四个通用寄存器会被切换到新任务的数据，所以要在这里保存（我们手动压的）
    uint32_t ebp;        // 用于保存切换时的EBP寄存器值
    uint32_t ebx;        // 用于保存切换时的EBX寄存器值
    uint32_t edi;        // 用于保存切换时的EDI寄存器值
    uint32_t esi;        // 用于保存切换时的ESI寄存器值

    // 首次上CPU的时候需要设置，后面就自动调整了
    // 在栈里，这就是所谓的返回地址，pop掉上面四个之后，ret的时候直接jmp这个地址，以此实现任务切换
    // 也就是直接跳入function(func_arg)里面了
    void (*ret_addr) (task_function* func, void* func_arg);
    // 这个返回地址是调用function(func_arg)的时候的返回地址（其实就是这里本身），对我们没什么用
    void (*unused_ret_addr);     // 占位用，不实际使用
    // 说白了就是传参
    task_function *function;    // 首次执行时调用的函数
    void* func_arg;             // 首次执行时调用函数的参数
};


// 创建任务，自动将其加入任务队列中，等待随时被调度（名字是全局唯一标识符，不得冲突）
struct task *task_create(char *name, int priority, task_function function, void* func_arg);
// 取消任务，将任务从链表中删除
void task_cancel(struct task *task);
// 通过名字获取任务
struct task *task_info(char *name);
// 当前正在运行的任务是哪个
struct task* running_task(void);
// 任务调度器
void task_schedule(void);
// 任务切换（在调度器切换会导致跳出中断处理程序，必须要在各个执行流主循环切换）
void task_switch(void);
// 多任务机制初始化
void init_multitasking(void);


#endif //HOS_KERNEL_THREAD_H
