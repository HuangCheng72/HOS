//
// Created by huangcheng on 2024/6/1.
//

#include "kernel_task.h"

#define PG_SIZE 0x1000

// 内核任务 TCB 的固定位置和栈位置
#define KERNEL_TCB ((struct task*)0x4009f000)

// 定义就绪队列和所有队列
static struct list_node ready_list;
static struct list_node all_task_list;

// 死亡任务队列（暂时还没关闭的任务都在里面）
static struct list_node died_list;

// 当前正在运行的任务
static struct task* current_task = NULL;
// 将要运行的任务（下一个任务）
static struct task* next_task = NULL;

// 初始化任务调度系统
void init_multitasking(void) {
    // 初始化三个队列
    init_list_node(&ready_list);
    init_list_node(&all_task_list);
    init_list_node(&died_list);

    // 初始化内核任务的 PCB（这一整页都直接清空）
    memset(KERNEL_TCB, 0, PG_SIZE);
    strcpy(KERNEL_TCB->name, "kernel");
    // 内核优先级为31，最高，运行时间也最长
    KERNEL_TCB->priority = 31;
    KERNEL_TCB->status = TASK_RUNNING;
    KERNEL_TCB->ticks = 31;
    KERNEL_TCB->elapsed_ticks = 0;
    // 内核栈位置固定的，之前设好了（现在假设是在栈底，后面调度的时候会自动更新的）
    KERNEL_TCB->self_stack = (uint8_t *)0xc0007000;
    // 魔数防止栈越界
    KERNEL_TCB->stack_magic = 0x20000702;
    // 因为现在肯定是内核在运行，所以不用加入到ready队列里面，加入到所有队列就行了
    init_list_node(&KERNEL_TCB->general_tag);
    list_add_tail(&KERNEL_TCB->all_task_tag, &all_task_list);

    // 内核的页表所在（为了方便切换）
    KERNEL_TCB->pgdir = PAGE_DIR_TABLE_POS + DRAM_OFFSET;
    // 内核的虚拟位图信息在内存管理初始化的时候写入

    // 将当前任务设置为内核任务
    current_task = KERNEL_TCB;
    // 还要运行很久，所以设置为下一个任务
    next_task = KERNEL_TCB;
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
