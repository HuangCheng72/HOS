//
// Created by huangcheng on 2024/6/3.
//

#include "kernel_sync.h"
#include "../kernel_interrupt/kernel_interrupt.h"
#include "kernel_task.h"

// 信号量等待者
struct semaphore_waiter {
    struct list_node tag;
    struct task *task;
};

// 初始化信号量
void semaphore_init(struct semaphore* psema, uint32_t value) {
    psema->value = value;
    init_list_node(&psema->waiters);
}

// 信号量的P操作（等待，信号量减1）
void semaphore_wait(struct semaphore *psema) {
    // 关闭中断，防止上下文切换
    enum intr_status old_status = intr_disable();

    // 没有使用权，必须阻塞
    // 用循环的原因是害怕解锁之后还是没得到资源使用权（被抢走了）
    // 所以要再判断一次
    while (psema->value == 0) {
        struct task *current = running_task();
        // 将当前任务作为一个等待者，插入等待队列
        struct semaphore_waiter waiter;
        waiter.task = current;
        init_list_node(&waiter.tag);

        list_add_tail(&waiter.tag, &psema->waiters); // 添加到等待队列
        task_block(TASK_BLOCKED); // 阻塞当前任务

    }
    // 获取使用权
    psema->value--;
    // 恢复先前状态
    intr_set_status(old_status);
}


// 信号量的V操作（信号，信号量加1）
void semaphore_signal(struct semaphore* psema) {
    // 关闭中断，防止上下文切换
    enum intr_status old_status = intr_disable();

    // 只要有
    if (!list_empty(&psema->waiters)) {
        // 解除第一个等待者的阻塞
        struct list_node *node = psema->waiters.next;
        struct semaphore_waiter *waiter = list_entry(node, struct semaphore_waiter, tag);
        list_del(node); // 从等待队列中删除

        // 结束阻塞，复活任务
        task_unblock(waiter->task);

    }

    // 释放使用权
    psema->value++;
    // 恢复先前状态
    intr_set_status(old_status);
}

// 互斥锁初始化
void mutex_init(struct mutex *pmutex) {
    semaphore_init(&pmutex->sema, 1);
}

// 互斥锁加锁
void mutex_lock(struct mutex *pmutex) {
    semaphore_wait(&pmutex->sema);
}

// 互斥锁解锁
void mutex_unlock(struct mutex *pmutex) {
    semaphore_signal(&pmutex->sema);
}

// 递归锁初始化
void recursive_mutex_init(struct recursive_mutex *pmutex) {
    semaphore_init(&pmutex->sema, 1);
    pmutex->owner = NULL;
    pmutex->recursive_count = 0;
}

// 递归锁加锁
void recursive_mutex_lock(struct recursive_mutex *pmutex) {
    struct task *current_task = running_task();
    if (pmutex->owner == current_task) {
        pmutex->recursive_count++;
    } else {
        semaphore_wait(&pmutex->sema);
        pmutex->owner = current_task;
        pmutex->recursive_count = 1;
    }
}

// 递归锁解锁
void recursive_mutex_unlock(struct recursive_mutex *pmutex) {
    if (pmutex->owner == running_task()) {
        if (--pmutex->recursive_count == 0) {
            pmutex->owner = NULL;
            semaphore_signal(&pmutex->sema);
        }
    }
}

// 读写锁初始化
void rwlock_init(struct rwlock *rw) {
    semaphore_init(&rw->read_lock, 1);
    semaphore_init(&rw->write_lock, 1);
    rw->readers = 0;
}

// 请求读锁
void rwlock_acquire_read(struct rwlock *rw) {
    semaphore_wait(&rw->read_lock);
    rw->readers++;
    if (rw->readers == 1) {
        semaphore_wait(&rw->write_lock);
    }
    semaphore_signal(&rw->read_lock);
}

// 释放读锁
void rwlock_release_read(struct rwlock *rw) {
    semaphore_wait(&rw->read_lock);
    rw->readers--;
    if (rw->readers == 0) {
        semaphore_signal(&rw->write_lock);
    }
    semaphore_signal(&rw->read_lock);
}

// 请求写锁
void rwlock_acquire_write(struct rwlock *rw) {
    semaphore_wait(&rw->write_lock);
}

// 释放写锁
void rwlock_release_write(struct rwlock *rw) {
    semaphore_signal(&rw->write_lock);
}
