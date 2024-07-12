//
// Created by huangcheng on 2024/6/1.
//

#ifndef HOS_KERNEL_TASK_H
#define HOS_KERNEL_TASK_H

#include "../../lib/lib_kernel/lib_kernel.h"
#include "../kernel_memory/kernel_memory.h"
#include "../kernel_interrupt/kernel_interrupt.h"

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

// 任务信息栈，用于保存一个任务切换时候的状态
struct task_info_stack {
    // 上次执行到的任务所有状态（返回地址，任务条件状态）
    uint32_t lr;     // 用于保存切换时的LR寄存器值
    uint32_t cpsr;   // 用于保存切换时的CPSR寄存器值

    // 首次上cpu的时候需要设置，因为arm的C调用约定前四个参数是寄存器传参
    // 使用函数包装器要使用前两个寄存器也就是r0和r1
    // 这两个要分别恢复到对应的寄存器才能传参成功，后续就不用管了（会自动保存）
    uint32_t r0;     // 用于保存切换时的R0寄存器值
    uint32_t r1;     // 用于保存切换时的R1寄存器值

    // 其他通用寄存器的值（按照压栈弹栈顺序）
    uint32_t r2;     // 用于保存切换时的R2寄存器值
    uint32_t r3;     // 用于保存切换时的R3寄存器值
    uint32_t r4;     // 用于保存切换时的R4寄存器值
    uint32_t r5;     // 用于保存切换时的R5寄存器值
    uint32_t r6;     // 用于保存切换时的R6寄存器值
    uint32_t r7;     // 用于保存切换时的R7寄存器值
    uint32_t r8;     // 用于保存切换时的R8寄存器值
    uint32_t r9;     // 用于保存切换时的R9寄存器值
    uint32_t r10;    // 用于保存切换时的R10寄存器值
    uint32_t r11;    // 用于保存切换时的R11寄存器值
    uint32_t r12;    // 用于保存切换时的R12寄存器值
};

// 多任务机制初始化
void init_multitasking(void);
// 通过名字获取任务
struct task *task_info(char *name);
// 当前正在运行的任务是哪个
struct task* running_task(void);
// 创建任务，自动将其加入任务队列中，等待随时被调度（名字是全局唯一标识符，不得冲突）
struct task *task_create(char *name, int priority, task_function function, void* func_arg);
// 取消任务，将任务从链表中删除
void task_cancel(struct task *task);
// 任务调度器
void task_schedule(void);
// 阻塞当前调用的任务，并修改任务为指定状态（如果想不调度，就挂起、阻塞、等待）
void task_block(enum task_status stat);
// 将该任务状态修改为TASK_READY，并插入到调度队列最前，即解除阻塞并使其尽快进入工作状态
void task_unblock(struct task *task);
// 让出CPU时间，但不阻塞
void task_yield(void);

#endif //HOS_KERNEL_TASK_H
