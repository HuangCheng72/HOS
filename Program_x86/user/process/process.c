//
// Created by huangcheng on 2024/6/5.
//

#include "process.h"

#include "../../kernel/kernel_gdt/kernel_gdt.h"
#include "../../kernel/kernel_page/kernel_page.h"
#include "../../devices/console/console.h"
#include "../../kernel/kernel_memory/kernel_memory.h"

#define PG_SIZE 0x1000

// 从虚拟地址获取物理地址
uint32_t virtual_to_physical_address(uint32_t virtual_addr) {

    /*
    // 在kernel_memory里面有用来分离的联合体结构，这里图方便用位运算直接分离
    uint32_t pde_index = (virtual_addr >> 22) & 0x3FF;  // 高10位：页目录索引
    uint32_t pte_index = (virtual_addr >> 12) & 0x3FF;  // 中10位：页表索引
    uint32_t offset = virtual_addr & 0xFFF;             // 低12位：页内偏移

    // 获取当前页目录表
    page_directory_entry_t *page_directory = (page_directory_entry_t *) PAGE_DIR_TABLE_POS;

    // 获取页表物理地址
    uint32_t page_table = page_directory[pde_index].table << 12;

    // 获取物理页框地址
    page_table_entry_t *pte = ((page_table_entry_t *) page_table + pte_index);
    uint32_t physical_page_addr = pte->frame << 12;

    // 计算实际物理地址
    uint32_t physical_addr = physical_page_addr | offset;

    // 返回结果
    return physical_addr;
     */

    // 以上是具体计算过程，以下是整合直接返回语句
    // 因为自引用之后0xfffff000就是当前页表位置，所以直接用0xfffff000来访问，不用记页表位置了
    return ((*(uint32_t *)( (*(uint32_t *)(0xfffff000 + (((virtual_addr >> 22) & 0x3ff) << 2)) & 0xfffff000)
                            + (((virtual_addr >> 12) & 0x3ff) << 2)) & 0xfffff000) | (virtual_addr & 0xfff));
}

// 创建用户空间页目录表（返回的是物理地址）
uint32_t create_user_page_dir(void) {
    // 这个函数是运行时候还是内核态，因此申请内核空间内存
    // 页表项本质上是一个4字节的无符号整数
    page_directory_entry_t *user_page_dir = malloc_page(KERNEL_FLAG, 1);

    if (user_page_dir == NULL) {
        return 0;
    }

    // 清理数据
    memset(user_page_dir, 0, PG_SIZE);

    // 初始化页目录表项
    // 低端映射是内核运行的必须，比如包括申请内存用到的位图，这部分必须全部包括在内
    // 这里映射第一个
    // 一个页目录表项管理4MB内存
    // 目前内核也就用到0x0012a000，足够覆盖了
    // 进程的虚拟地址是从128MB开始的，前面那么多足够用了
    // 如果图省事，直接映射前128MB低端内存，那就是32个页目录表项
    // 但是这个内核没有Linux那么大，需要的内存估计不会超过8MB，差不多得了

    // 只要最后一项自引用了，0xfffff000就是当前页目录表所在的地方，找页目录表地址就很方便了，直接变成了常量
    user_page_dir[0] = ((page_directory_entry_t *)0xfffff000)[0];

    // 复制768项到1024项，访问内核的全部资源
    // 这样子，每个进程的页表，虚拟地址0xc0000000到0xffffffff这部分，共享同样的映射
    // 这样不管是内核还是每个进程，都可以用同样的virtual_to_physical_address来转换虚拟地址到物理地址了

    for(int i = 768; i < 1024; i++) {
        // 只要最后一项自引用了，0xfffff000就是当前页目录表所在的地方，找页目录表地址就很方便了，直接变成了常量
        user_page_dir[i] = ((page_directory_entry_t *)0xfffff000)[i];
    }
    // 计算该页的物理地址
    uint32_t user_page_table_phy_addr = virtual_to_physical_address((uint32_t)user_page_dir);
    // 页目录表最后一项自引用
    // 其作用是快速寻址到页目录表
    // 只要最后一项自引用了，0xfffff000就是当前页目录表所在的地方，找页目录表地址就很方便了，直接变成了常量
//    user_page_dir[1023].present = 1;
//    user_page_dir[1023].us = 0;
//    user_page_dir[1023].rw = 1;
    user_page_dir[1023].table = user_page_table_phy_addr >> 12;

    // 只有物理地址才能载入cr3，因此必须返回物理地址
    return user_page_table_phy_addr;
}

// 向上取整宏，将 x 向上取整到 align 的倍数
#define DIV_ROUND_UP(x, align) (((x) + (align) - 1) / (align))

// 创建并初始化用户进程虚拟地址位图（这个动作是在内核空间进行的，所以必须申请内核空间的页）
uint8_t init_user_virtual_addr(struct task *user_process) {
    if(user_process == NULL) {
        return 0;
    }

    // 计算要多少页才装得下
    //uint32_t bitmap_pg_cnt = DIV_ROUND_UP((0xc0000000 - USER_VADDR_START) / PG_SIZE / 8 , PG_SIZE);
    // 我这不用中间变量了，直接算
    user_process->process_virtual_address.bitmap_virtual_memory.bits = malloc_page(KERNEL_FLAG, DIV_ROUND_UP((0xc0000000 - USER_VADDR_START) / PG_SIZE / 8 , PG_SIZE));
    if(user_process->process_virtual_address.bitmap_virtual_memory.bits == NULL) {
        return 0;
    }
    // 对位图进行设置
    user_process->process_virtual_address.virtual_addr_start = USER_VADDR_START;
    // 位图长度
    user_process->process_virtual_address.bitmap_virtual_memory.btmp_bytes_len = (0xc0000000 - USER_VADDR_START) / PG_SIZE / 8;
    // 位图初始化
    bitmap_init(&user_process->process_virtual_address.bitmap_virtual_memory);

    return 1;
}

// 给出一个中断栈数据，跳入用户态
extern void switch_to_user_mode();

// x86平台从内核线程到用户进程，普遍做法是用iretd指令让CPU自动进入用户态
// switch_to_user_mode就是这么编写的
// 本质上和线程切换任务的方式类似，让iretd指令帮助我们自动跳进入口函数

// 用一个包装函数，来做初始化工作，辅助进入用户态
void wrap_task_to_process() {

    // 先禁止中断，我怕设置不完成
    enum intr_status old_status = intr_disable();
    // 把之前没完成的设置完成了，申请一个页作为用户态进程栈
    struct interrupt_stack* int_stack = (struct interrupt_stack*)((uint32_t)(running_task()) + PG_SIZE - sizeof(struct interrupt_stack));

    // 之前把这个操作完善了，这会在用户内存池（16MB以后申请内存空间，并且映射到用户进程自己的页表上）
    void *user_stack = malloc_page(USER_FLAG, 1);
    if(!user_stack) {
        // 申请不了，回滚
        task_cancel(running_task());
        intr_set_status(old_status);
        return;
    }
    int_stack->user_esp = (uint32_t)user_stack + PG_SIZE;
    // 把这个设置完成了之后就可以进入用户态了
    switch_to_user_mode();
}

struct task* process_create(char *name, uint32_t entry_function){
    // 关闭中断
    enum intr_status old_status = intr_disable();

    // 创建一个任务，这个任务其实就是跳进用户态
    // 真正的入口函数在中断栈里面
    // 默认进程和内核平等，内核优先级是31
    struct task* new_task = task_create(name, 31, wrap_task_to_process, NULL);
    if (new_task == NULL) {
        intr_set_status(old_status);
        return NULL;
    }

    // 设置用户进程的页目录表
    new_task->pgdir = create_user_page_dir();
    if (!new_task->pgdir) {
        task_cancel(new_task);
        intr_set_status(old_status);
        return NULL;
    }

    // 创建并初始化用户进程虚拟地址位图
    if(init_user_virtual_addr(new_task) == 0) {
        return NULL;
    }

    // 加载用户进程，设置入口点
    // 这么做是为了以后扩展到能从文件里面加载入口函数
    uint32_t entry_point = entry_function; // entry_function 作为入口函数
    if (entry_point == 0) {
        task_cancel(new_task);
        intr_set_status(old_status);
        return NULL;
    }

    // 设置中断栈
    // 因为创建任务的时候，先预留了中断栈，再预留了任务信息栈，所以现在self_stack指向的是任务信息栈的开头
    // 越过任务信息栈，才到中断栈
    struct interrupt_stack* int_stack = (struct interrupt_stack*)((uint32_t)(new_task->self_stack) + sizeof(struct task_info_stack));

    // 中断栈的意义就在于可以通过伪装成中断恢复的形式，从内核态跳入用户态
    int_stack->eip = entry_point; // iretd的时候，就会直接从这里跳进用户进程的入口函数了
    int_stack->cs = SELECTOR_U_CODE;
    int_stack->eflags = 0x202; // IF = 1, IOPL = 0

    int_stack->ds = SELECTOR_U_DATA;
    int_stack->es = SELECTOR_U_DATA;
    int_stack->fs = SELECTOR_U_DATA;
    int_stack->gs = 0;  // 用户态不需要这个，也不能有这个

    int_stack->esp = (uint32_t)int_stack + 14 * 4;  // 以防万一，我把这个esp的值设置到CPU自动压栈的错误码位置（这个其实cpu会自动调整）

    int_stack->user_ss = SELECTOR_U_STACK;   // 内核态和用户态的栈段选择子用的都是数据段选择子
    // 这个操作挪到进程包装函数里面了
    //int_stack->user_esp = (uint32_t)new_task + PG_SIZE; // 与TSS保持一致

    // 恢复中断，等待调度
    intr_set_status(old_status);

    return new_task;
}
