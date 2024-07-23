//
// Created by huangcheng on 2024/6/1.
//

#include "kernel_task.h"

#define PG_SIZE 0x1000

// 内核任务 TCB 的固定位置和栈位置（注意JZ2440的SDRAM偏移量是0x30000000）
#define KERNEL_TCB ((struct task*)0x3009f000)

// 定义就绪队列和所有队列
static struct list_node ready_list;
static struct list_node all_task_list;

// 死亡任务队列（暂时还没关闭的任务都在里面）
static struct list_node died_list;

// 当前正在运行的任务
static struct task* current_task = NULL;
// 将要运行的任务（下一个任务）
static struct task* next_task = NULL;

// 切换任务汇编
extern void switch_to(struct task *cur, struct task *next);

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
    // 中断栈是用来跳入用户态用的，离得还远，先放一边去，说不定也不用

    // 预留任务信息栈位置
    new_task->self_stack -= sizeof(struct task_info_stack);

    // 内核线程默认为和内核用同样的资源（如果要创建为用户进程分配新的资源，再自己修改）
    new_task->pgdir = KERNEL_TCB->pgdir;
    // 位图也复制内核的位图（结构体等于运算直接浅拷贝）
    new_task->process_virtual_address = KERNEL_TCB->process_virtual_address;

    // 这些说白了是任务第一次上CPU运行的时候需要设置的，后面就会自动保存新的
    // 寄存器传参，所以要设置r0和r1（函数包装器的参数），其他寄存器都是0即可
    ((struct task_info_stack*)new_task->self_stack)->r0 = (uint32_t)function;
    ((struct task_info_stack*)new_task->self_stack)->r1 = (uint32_t)func_arg;
    // 任务的返回地址就是自己（因为切换命令是subs    pc, lr, #4，为了保险可以加上4，反正只用一次，第二次就是自动保存的了）
    ((struct task_info_stack*)new_task->self_stack)->lr = (uint32_t)(&func_wrapper) + 4;
    // cpsr指定为管理模式（内核态）
    ((struct task_info_stack*)new_task->self_stack)->cpsr = 0x13;

    // 将任务加入调度队列和所有队列
    list_add(&new_task->general_tag, &ready_list);
    list_add(&new_task->all_task_tag, &all_task_list);

    return new_task;
}

// 取消任务（线程不回收资源，页表和位图进程自己回收）
void task_cancel(struct task* task) {
    if(task == NULL) {
        return;
    }
    if(task == current_task) {
        // 这里要等任务完成才能删除
        // 标记其等待删除，让任务调度器删掉
        task->status = TASK_DIED;
        // current_task本来就不应该在ready_list里面
        // 但是怕了，还是先删一遍
        list_del(&task->general_tag);
        // 加入到死亡队列
        list_add_tail(&task->general_tag, &died_list);
        return;
    }
    if(task == KERNEL_TCB) {
        // 内核任务不得删除
        return;
    }
    // 关闭中断防止切换
    enum intr_status old_status = intr_disable();
    if(next_task == task) {
        // 碰上这种情况，直接切成current
        next_task = current_task;
    }
    list_del(&task->general_tag);
    list_del(&task->all_task_tag);
    free_page(task, 1);
    // 恢复先前状态
    intr_set_status(old_status);
}

void task_schedule(void) {
    // 检查栈溢出
    if (current_task->stack_magic != 0x20000702) {
        // 处理栈溢出错误
        for(;;);
    }

    // 清理死亡队列里的任务
    struct list_node *pos;
    struct task* tmp_task;
    list_for_each(pos, &died_list) {
        tmp_task = list_entry(pos, struct task, general_tag);
        list_del(&tmp_task->general_tag);
        list_del(&tmp_task->all_task_tag);
        // 释放TCB
        free_page(tmp_task, 1);
    }

    if (current_task->status == TASK_RUNNING) { // 若此线程只是cpu时间片到了,将其加入到就绪队列尾
        if(list_empty(&ready_list)) {
            // 只有一个线程的特殊情况处理，只重置时间，减少开销
            // 重新将当前线程的ticks再重置为其priority;
            current_task->ticks = current_task->priority;
            // 其他的什么都不要变，直接return
            return;
        }
        list_add_tail(&current_task->general_tag, &ready_list);
        current_task->ticks = current_task->priority;     // 重新将当前线程的ticks再重置为其priority;
        current_task->status = TASK_READY;
    } else {
        // 我怕你没删除，所以我无论如何都要删除一遍
        // 总之你不是task_running，你就是其他的，那不准进入ready_list
        // 所以保证不得留在ready_list里面

        // 这里发现问题了，task_yield里面本来已经插到末尾了，这里如果不判断的话，会出问题，彻底丢失无法调度了
        if(current_task->status != TASK_READY) {
            list_del(&current_task->general_tag);
        }
        // 主动让出使用权不得删除
        // 其实不删除也行，但我觉得还是删除比较保险
    }

    if(list_empty(&ready_list)) {
        // 没任务，继续运行，不要切换
        return;
    } else {
        // 取第一个结点
        next_task = list_entry(ready_list.next, struct task, general_tag);
        next_task->status = TASK_RUNNING;

        // 因为切换之后会直接跳出中断处理程序，所以绝不能在这里切换，必须要在中断切换
    }
}

// 任务切换，在IRQ处理函数中调用
void task_switch(void){
    if ( current_task == next_task ) {
        // 无需切换，直接返回，不要浪费资源
        return;
    }
    // 从ready链表中删除结点
    list_del(&next_task->general_tag);
    // 如果不切换就跳不出来了，所以用一个临时变量来中继
    struct task *cur_task = current_task;
    current_task = next_task;

    // 还没移植进程，切换页表的事情先放一边

    // 真正切换任务了，进去就暂时出不来了
    switch_to(cur_task, next_task);
}

// 阻塞当前调用的任务，并修改任务为指定状态（如果想不调度，就挂起、阻塞、等待）
void task_block(enum task_status stat) {
    // 为了配合新的switch_to函数
    // 要在这里保存上下文到指定位置，也就是IRQ栈位置（这里没进入中断，只能手动更新，不然就是先前的）
    // 不能调用函数，怕lr改变，只能用内联汇编写
    // 因为怕cpsr的条件状态改变，所以直接ldr，这样不会有条件状态位改变的问题
    // 进入这里的时候，lr保存的位置是调用task_yield命令的下一条（也就是pc + 4）
    // 因为切换命令是subs    pc, lr, #4，就会无限进入task_yield
    // 所以需要修改，再增加一个偏移量
    // 因为进入switch_to之前会经过几个函数，有不少临时变量都在栈上
    // 也可以直接用新的栈，每次都是新栈，这样就不怕出事了，类似于irq_handler，而且还不用清理

    // 这里先记录到约定位置，注意arm的C语言调用约定寄存器传参问题
    asm volatile("push  {r12}\n"
                 "ldr   r12, =0xc0007ff8\n"
                 "str   r11, [r12]\n"
                 "ldr   r12, =0xc0007ff4\n"
                 "str   r10, [r12]\n"
                 "ldr   r12, =0xc0007ff0\n"
                 "str   r9, [r12]\n"
                 "ldr   r12, =0xc0007fec\n"
                 "str   r8, [r12]\n"
                 "ldr   r12, =0xc0007fe8\n"
                 "str   r7, [r12]\n"
                 "ldr   r12, =0xc0007fe4\n"
                 "str   r6, [r12]\n"
                 "ldr   r12, =0xc0007fe0\n"
                 "str   r5, [r12]\n"
                 "ldr   r12, =0xc0007fdc\n"
                 "str   r4, [r12]\n"
                 "ldr   r12, =0xc0007fd8\n"
                 "str   r3, [r12]\n"
                 "ldr   r12, =0xc0007fd4\n"
                 "str   r2, [r12]\n"
                 "ldr   r12, =0xc0007fd0\n"
                 "str   r1, [r12]\n"
                 "ldr   r12, =0xc0007fcc\n"
                 "str   r0, [r12]\n"
                 "ldr   r11, =0xc0007ffc\n"
                 "pop   {r12}\n"
                 "str   r12, [r11]\n"
                 "ldr   r12, =0xc0007fc8\n"
                 "mrs   r11, cpsr\n"
                 "str   r11, [r12]\n"
                 "ldr   r12, =0xc0007fc4\n"
                 "add   r11, lr, #4\n"
                 "str   r11, [r12]\n"
                 "ldr   r12, =0xc0008080\n"
                 "str   sp, [r12]\n"
                 "ldr   sp, =0xc0007800\n"
    );

    // 关闭中断，保存先前状态
    enum intr_status old_status = intr_disable();
    // 获取现在正在进行的任务（可能更新不及时，注意）
    struct task *cur_task = current_task;
    // 修改状态
    cur_task->status = stat;
    // 调度新任务
    task_schedule();
    // 手动强行调度
    task_switch();
    // 任务阻塞解除才会调用这句重设状态语句
    intr_set_status(old_status);
}

void task_unblock(struct task *task) {
    // 惯例，关中断
    enum intr_status old_status = intr_disable();
    // 只要不是READY或者RUNNING（怕出意外），都重设为READY
    if (task->status != TASK_READY && task->status != TASK_RUNNING) {
        // 检查是否已经在ready_list中
        struct list_node *pos;
        list_for_each(pos, &ready_list) {
            if (pos == &task->general_tag) {
                put_str("Task already in ready_list\n");
                for(;;);
            }
        }

        // 我怕你没删除，所以我无论如何都要删除一遍
        list_del(&task->general_tag);
        // 插入任务到ready_list队头，尽快被调度
        list_add(&task->general_tag, &ready_list);
        task->status = TASK_READY;

    }
    // 操作完成重设状态（谁知道原来是不是也是关中断）
    intr_set_status(old_status);
}

// 让出CPU时间，但不阻塞
void task_yield() {
    // 为了配合新的switch_to函数
    // 要在这里保存上下文到指定位置，也就是IRQ栈位置（这里没进入中断，只能手动更新，不然就是先前的）
    // 不能调用函数，怕lr改变，只能用内联汇编写
    // 因为怕cpsr的条件状态改变，所以直接ldr，这样不会有条件状态位改变的问题
    // 进入这里的时候，lr保存的位置是调用task_yield命令的下一条（也就是pc + 4）
    // 因为切换命令是subs    pc, lr, #4，就会无限进入task_yield
    // 所以需要修改，再增加一个偏移量
    // 因为进入switch_to之前会经过几个函数，有不少临时变量都在栈上
    // 也可以直接用新的栈，每次都是新栈，这样就不怕出事了，类似于irq_handler，而且还不用清理

    // 这里先记录到约定位置，注意arm的C语言调用约定寄存器传参问题
    asm volatile("push  {r12}\n"
                 "ldr   r12, =0xc0007ff8\n"
                 "str   r11, [r12]\n"
                 "ldr   r12, =0xc0007ff4\n"
                 "str   r10, [r12]\n"
                 "ldr   r12, =0xc0007ff0\n"
                 "str   r9, [r12]\n"
                 "ldr   r12, =0xc0007fec\n"
                 "str   r8, [r12]\n"
                 "ldr   r12, =0xc0007fe8\n"
                 "str   r7, [r12]\n"
                 "ldr   r12, =0xc0007fe4\n"
                 "str   r6, [r12]\n"
                 "ldr   r12, =0xc0007fe0\n"
                 "str   r5, [r12]\n"
                 "ldr   r12, =0xc0007fdc\n"
                 "str   r4, [r12]\n"
                 "ldr   r12, =0xc0007fd8\n"
                 "str   r3, [r12]\n"
                 "ldr   r12, =0xc0007fd4\n"
                 "str   r2, [r12]\n"
                 "ldr   r12, =0xc0007fd0\n"
                 "str   r1, [r12]\n"
                 "ldr   r12, =0xc0007fcc\n"
                 "str   r0, [r12]\n"
                 "ldr   r11, =0xc0007ffc\n"
                 "pop   {r12}\n"
                 "str   r12, [r11]\n"
                 "ldr   r12, =0xc0007fc8\n"
                 "mrs   r11, cpsr\n"
                 "str   r11, [r12]\n"
                 "ldr   r12, =0xc0007fc4\n"
                 "add   r11, lr, #4\n"
                 "str   r11, [r12]\n"
                 "ldr   r12, =0xc0008080\n"
                 "str   sp, [r12]\n"
                 "ldr   sp, =0xc0007800\n"
    );

    // 关闭中断，防止任务切换过程中出现竞态条件
    enum intr_status old_status = intr_disable();

    // 当前任务重新加入到就绪队列的队尾
    if (current_task->status == TASK_RUNNING) {
        list_add_tail(&current_task->general_tag, &ready_list);
        current_task->status = TASK_READY;
    }

    // 调度下一个任务
    task_schedule();
    // 执行任务切换
    task_switch();
    // 恢复中断状态
    intr_set_status(old_status);
}
