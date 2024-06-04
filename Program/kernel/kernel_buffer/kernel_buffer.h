//
// Created by huangcheng on 2024/6/4.
//

#ifndef HOS_KERNEL_BUFFER_H
#define HOS_KERNEL_BUFFER_H

#include "../../lib/lib_kernel/lib_kernel.h"
#include "../kernel_task/kernel_sync.h"

// 这是一个基于生产者-消费者模型的 内核级 线程安全 环形 缓冲区的实现
// 生产者写入数据的时候，消费者被阻塞
// 消费者读数据的时候，生产者被阻塞
// 内核缓冲区的结构体
struct kernel_buffer {
    uint8_t *data;                // 实际的数据存储
    uint32_t size;             // 缓冲区大小
    uint32_t read_pos;         // 读位置
    uint32_t write_pos;        // 写位置

    struct mutex mutex;        // 互斥锁，保护缓冲区访问
    struct semaphore writable; // 可写信号量，表示空闲空间数量
    struct semaphore readable; // 可读信号量，表示已填充数据数量
};

// 获取一个初始化好的内核缓冲区
struct kernel_buffer* kernel_buffer_create(void);
// 回收缓冲区
void kernel_buffer_free(struct kernel_buffer *k_buf);
// 将data的数据写size个字节到内核缓冲区，size不得大于4000，建议在2048以内（由生产者调用，写时消费者被阻塞）
void kernel_buffer_write(struct kernel_buffer *k_buf, char *data, uint32_t size);
// 从内核缓冲区读取数据，读size个字节到data，size不得大于4000，建议在2048以内（由消费者调用，读时生产者被阻塞）
void kernel_buffer_read(struct kernel_buffer *k_buf, char *data, uint32_t size);


#endif //HOS_KERNEL_BUFFER_H
