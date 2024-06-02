//
// Created by huangcheng on 2024/6/1.
//

#include "kernel_task.h"
#include "../kernel_interrupt/kernel_interrupt.h"
#include "../kernel_memory/kernel_memory.h"

#define PG_SIZE 0x1000

// 内核任务 TCB 的固定位置和栈位置
#define KERNEL_TCB ((struct task*)0x9f000)

// 切换任务汇编
extern void switch_to(struct task *cur, struct task *next);

// 定义就绪队列和所有队列
static struct list_node ready_list;
static struct list_node all_task_list;

// 当前正在运行的任务
static struct task* current_task = NULL;
// 将要运行的任务（下一个任务）
static struct task* next_task = NULL;

// 初始化任务调度系统
void init_multitasking(void) {
    init_list_node(&ready_list);
    init_list_node(&all_task_list);

    // 初始化内核任务的 PCB（这一整页都直接清空）
    memset(KERNEL_TCB, 0, PG_SIZE);
    strcpy(KERNEL_TCB->name, "kernel");
    // 内核优先级为31，最高，运行时间也最长
    KERNEL_TCB->priority = 31;
    KERNEL_TCB->status = TASK_RUNNING;
    KERNEL_TCB->ticks = 31;
    KERNEL_TCB->elapsed_ticks = 0;
    // 内核栈位置固定的，之前设好了（现在假设是在栈底，后面调度的时候会自动更新的）
    KERNEL_TCB->self_stack = (uint8_t *)0xc0001500;
    // 魔数防止栈越界
    KERNEL_TCB->stack_magic = 0x20000702;
    // 因为现在肯定是内核在运行，所以不用加入到ready队列里面，加入到所有队列就行了
    init_list_node(&KERNEL_TCB->general_tag);
    list_add_tail(&KERNEL_TCB->all_task_tag, &all_task_list);
    // 将当前任务设置为内核任务
    current_task = KERNEL_TCB;
    // 还要运行很久，所以设置为下一个任务
    next_task = KERNEL_TCB;
}

// 函数包装器
void func_wrapper(task_function* func, void* arg) {
    func(arg);
}

// 创建任务
struct task* task_create(char* name, int priority, task_function function, void* func_arg) {
    struct task* new_task = (struct task*)malloc_page(KERNEL_FLAG, 1);
    if (new_task == NULL) {
        return NULL;  // 内存分配失败
    }
    // 清空整页，这样后面就不用清理了
    memset(new_task, 0, PG_SIZE);
    strcpy(new_task->name, name);
    new_task->priority = priority;
    new_task->status = TASK_READY;
    new_task->ticks = priority;
    new_task->elapsed_ticks = 0;
    new_task->stack_magic = 0x20000702;

    // 栈是从高地址往低地址发展的
    // 所以最开始的栈顶设置在栈底
    new_task->self_stack = ((uint8_t*)new_task + PG_SIZE);
    // 预留中断栈位置（栈顶往低地址移动）
    new_task->self_stack -= sizeof(struct interrupt_stack);
    // 预留任务信息栈位置
    new_task->self_stack -= sizeof(struct task_info_stack);

    // 其他的都清空了，现在不用初始化

    // 这些说白了是任务第一次上CPU运行的时候需要设置的，后面就会自动保存新的
    ((struct task_info_stack*)new_task->self_stack)->ret_addr = &func_wrapper;
    ((struct task_info_stack*)new_task->self_stack)->function = function;
    ((struct task_info_stack*)new_task->self_stack)->func_arg = func_arg;

    // 将任务加入调度队列和所有队列
    list_add(&new_task->general_tag, &ready_list);
    list_add(&new_task->all_task_tag, &all_task_list);

    return new_task;
}

// 取消任务
void task_cancel(struct task* task) {
    if(task == current_task) {
        // 这里要等任务完成才能删除，不知道该怎么写，先return了
        return;
    }
    list_del(&task->general_tag);
    list_del(&task->all_task_tag);
    free_page(KERNEL_FLAG, task, 1);
}

// 通过名字获取任务
struct task* task_info(char *name) {
    LIST_NODE* pos;
    list_for_each(pos, &all_task_list) {
        struct task* task = list_entry(pos, struct task, all_task_tag);
        if (strcmp(task->name, name) == 0) {
            return task;
        }
    }
    return NULL;
}

// 当前正在运行的任务
struct task* running_task(void) {
    return current_task;
}

void task_schedule(void) {
    // 检查栈溢出
    if (current_task->stack_magic != 0x20000702) {
        // 处理栈溢出错误
        for(;;);
    }

    if (current_task->status == TASK_RUNNING) { // 若此线程只是cpu时间片到了,将其加入到就绪队列尾
        list_add_tail(&current_task->general_tag, &ready_list);
        current_task->ticks = current_task->priority;     // 重新将当前线程的ticks再重置为其priority;
        current_task->status = TASK_READY;
    } else {
        /* 若此线程需要某事件发生后才能继续上cpu运行,
        不需要将其加入队列,因为当前线程不在就绪队列中。*/
    }

    if(list_empty(&ready_list)) {
        // 没任务，继续运行，不要切换
        return;
    } else {
        // 取第一个结点
        next_task = list_entry(ready_list.next, struct task, general_tag);
        next_task->status = TASK_RUNNING;

        // 因为切换之后会直接跳出中断处理程序，所以绝不能在这里切换，必须要在内核主循环切换
    }
}

// 任务切换（在调度器切换会导致跳出中断处理程序，必须要在各个执行流主循环切换）
void task_switch(void){
    if (current_task == next_task) {
        // 无需切换，直接返回，不要浪费资源
        return;
    }
    // 从ready链表中删除结点
    list_del(&next_task->general_tag);
    // 如果不切换就跳不出来了，所以用一个临时变量来中继
    struct task *cur_task = current_task;
    current_task = next_task;

    // 真正切换任务了，进去就暂时出不来了
    switch_task(cur_task, next_task);
}
