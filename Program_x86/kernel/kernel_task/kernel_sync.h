//
// Created by huangcheng on 2024/6/3.
//

#ifndef HOS_KERNEL_SYNC_H
#define HOS_KERNEL_SYNC_H

#include "../../lib/lib_kernel/lib_kernel.h"

// 最基本的信号量结构
struct semaphore {
    uint32_t value;                 // 信号量的值
    struct list_node waiters;       // 等待队列，阻塞在此信号量上的任务
};

// 信号量初始化
void semaphore_init(struct semaphore *psema, uint32_t value);
// 信号量等待（P操作，减去1，获取控制权）
void semaphore_wait(struct semaphore *psema);
// 信号量增加（V操作，加上1，让出控制权）
void semaphore_signal(struct semaphore *psema);

// 互斥锁结构
struct mutex {
    struct semaphore sema;
};

// 互斥锁初始化
void mutex_init(struct mutex *pmutex);
// 互斥锁加锁
void mutex_lock(struct mutex *pmutex);
// 互斥锁解锁
void mutex_unlock(struct mutex *pmutex);


// 递归锁结构
struct recursive_mutex {
    struct semaphore sema;
    struct task *owner;
    uint32_t recursive_count;
};

// 递归锁初始化
void recursive_mutex_init(struct recursive_mutex *pmutex);
// 递归锁加锁
void recursive_mutex_lock(struct recursive_mutex *pmutex);
// 递归锁解锁
void recursive_mutex_unlock(struct recursive_mutex *pmutex);

// 读写锁结构
struct rwlock {
    struct semaphore read_lock;
    struct semaphore write_lock;
    int readers;
};

// 读写锁初始化
void rwlock_init(struct rwlock *rw);
// 请求读锁
void rwlock_acquire_read(struct rwlock *rw);
// 释放读锁
void rwlock_release_read(struct rwlock *rw);
// 请求写锁
void rwlock_acquire_write(struct rwlock *rw);
// 释放写锁
void rwlock_release_write(struct rwlock *rw);

#endif //HOS_KERNEL_SYNC_H
