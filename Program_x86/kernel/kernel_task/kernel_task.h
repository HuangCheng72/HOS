//
// Created by huangcheng on 2024/6/1.
//

#ifndef HOS_KERNEL_TASK_H
#define HOS_KERNEL_TASK_H

#include "../../lib/lib_kernel/lib_kernel.h"
#include "../kernel_memory/kernel_memory.h"

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

//TCB（任务控制块），表明这是一个任务（不管是进线程，都是任务）
struct task {
    uint8_t *self_stack;	    // 各任务都用自己的栈（指向栈顶）

    enum task_status status;    // 任务状态，这个不用说
    char name[64];              // 任务名（未来可以改成全局唯一ID）
    uint8_t priority;           // 优先级
    uint8_t ticks;	            // 每次在处理器上执行的时间嘀嗒数（一个ticks即定时器IRQ0中断一次，决定什么时候调度任务）

    uint32_t elapsed_ticks;     // 此任务自上cpu运行后至今占用了多少cpu嘀嗒数,

    struct list_node general_tag;   // 当前串在哪个调度队列上
    struct list_node all_task_tag;  // 所有任务都串在一起，这是为了保证任何时候都能找到

    uint32_t pgdir;            // 页表的物理地址（单独分配这个资源的时候就是进程了）
    Virtual_Addr process_virtual_address;  // 用户进程的虚拟地址资源，申请内存的时候需要
    uint32_t stack_magic;	    // 用这串数字做栈的边界标记,用于检测任务栈的溢出
};

// 中断栈数据（叫中断栈，一个是因为它真的存储中断数据，而是用其模拟中断处理恢复上下文的手段进入用户态）
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
    uint32_t esp;        // 栈指针寄存器 ESP（这个没什么用，用不到的）
    uint32_t ebx;        // 通用寄存器 EBX
    uint32_t edx;        // 通用寄存器 EDX
    uint32_t ecx;        // 通用寄存器 ECX
    uint32_t eax;        // 通用寄存器 EAX
    uint32_t error_code; // CPU自动压栈的错误码 (可能有，也可能没有，但是从内核态跳入用户态的时候绝对没有)
    uint32_t eip;        // 指令指针寄存器
    uint32_t cs;         // 代码段寄存器
    uint32_t eflags;     // 标志寄存器
    uint32_t user_esp;   // 低特权级进入高特权级的时候CPU自动压的栈指针 ESP
    uint32_t user_ss;    // 低特权级进入高特权级的时候CPU自动压的栈段选择子 SS
};


// 任务信息栈，用于保存一个任务切换时候的状态
struct task_info_stack {
    // 为了保存用户状态，需要保存八个通用寄存器的值
    uint32_t edi;        // 用于保存切换时的EDI寄存器值
    uint32_t esi;        // 用于保存切换时的ESI寄存器值
    uint32_t ebp;        // 用于保存切换时的EBP寄存器值
    uint32_t esp;        // 用于保存切换时的ESP寄存器值（这个没什么用，cpu会自动调整）
    uint32_t ebx;        // 用于保存切换时的EBX寄存器值
    uint32_t edx;        // 用于保存切换时的EDX寄存器值
    uint32_t ecx;        // 用于保存切换时的ECX寄存器值
    uint32_t eax;        // 用于保存切换时的EAX寄存器值

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
// 多任务机制初始化
void init_multitasking(void);
// 阻塞当前调用的任务，并修改任务为指定状态（如果想不调度，就挂起、阻塞、等待）
void task_block(enum task_status stat);
// 将该任务状态修改为TASK_READY，并插入到调度队列最前，即解除阻塞并使其尽快进入工作状态
void task_unblock(struct task *task);
// 让出CPU时间，但不阻塞
void task_yield(void);

#endif //HOS_KERNEL_TASK_H
