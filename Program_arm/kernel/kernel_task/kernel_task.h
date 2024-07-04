//
// Created by huangcheng on 2024/6/1.
//

#ifndef HOS_KERNEL_TASK_H
#define HOS_KERNEL_TASK_H

#include "../../lib/lib_kernel/lib_kernel.h"
#include "../kernel_memory/kernel_memory.h"

// 只是为了初始化内核的TCB和任务管理机制
// 所以其他的暂时不移植

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

// 多任务机制初始化
void init_multitasking(void);
// 通过名字获取任务
struct task *task_info(char *name);
// 当前正在运行的任务是哪个
struct task* running_task(void);

#endif //HOS_KERNEL_TASK_H
