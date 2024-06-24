//
// Created by huangcheng on 2024/6/4.
//

#include "kernel_buffer.h"
#include "../kernel_memory/kernel_memory.h"

#define PAGE_SIZE 0x1000

struct kernel_buffer* kernel_buffer_create(void) {
    // 申请一整页4KB内存，起始地址放缓冲区控制信息，剩下的都作为数据区使用
    void *page_addr = malloc_page(KERNEL_FLAG,1);
    if (page_addr == NULL) {
        return NULL; // 分配失败，返回NULL
    }

    struct kernel_buffer *k_buf = (struct kernel_buffer *)page_addr;
    k_buf->data = ((uint8_t *)page_addr + sizeof(struct kernel_buffer));
    k_buf->read_pos = 0;
    k_buf->write_pos = 0;
    k_buf->is_full = 0;
    k_buf->size = PAGE_SIZE - sizeof(struct kernel_buffer);
    mutex_init(&k_buf->mutex);
    semaphore_init(&k_buf->writable, k_buf->size);      // 初始空闲空间为缓冲区大小
    semaphore_init(&k_buf->readable, 0);        // 初始已填充数据为0

    return k_buf;
}

void kernel_buffer_free(struct kernel_buffer *k_buf) {
    // 用位运算方式取低12位，如果低12位全部是0，那么就是有效的，可以回收
    // NULL的情况，free_page里面有判断，不用再判断了
    if(((uint32_t)k_buf & 0xFFF) == 0) {
        free_page((void *)k_buf, 1);
    }
}

// 写入缓冲区
void kernel_buffer_write(struct kernel_buffer *k_buf, char *data, uint32_t size) {
    if (size > k_buf->size) {
        // 边界检查，一次性写不能超过缓冲区大小
        return;
    }
    for (uint32_t i = 0; i < size; i++) {
        semaphore_wait(&k_buf->writable);   // 等待可写空间
        mutex_lock(&k_buf->mutex);          // 锁定缓冲区

        k_buf->data[k_buf->write_pos] = data[i];
        k_buf->write_pos = (k_buf->write_pos + 1) % k_buf->size;

        k_buf->is_full = (k_buf->write_pos == k_buf->read_pos); // 更新is_full标志

        mutex_unlock(&k_buf->mutex);        // 解锁缓冲区
        semaphore_signal(&k_buf->readable); // 通知有新数据可读
    }
}

// 读取缓冲区
void kernel_buffer_read(struct kernel_buffer *k_buf, char *data, uint32_t size) {
    if (size > k_buf->size) {
        // 边界检查，一次性读不能超过缓冲区大小
        return;
    }
    for (uint32_t i = 0; i < size; i++) {
        semaphore_wait(&k_buf->readable);   // 等待可读数据
        mutex_lock(&k_buf->mutex);          // 锁定缓冲区

        data[i] = k_buf->data[k_buf->read_pos];
        k_buf->read_pos = (k_buf->read_pos + 1) % k_buf->size;

        k_buf->is_full = 0; // 更新is_full标志

        mutex_unlock(&k_buf->mutex);        // 解锁缓冲区
        semaphore_signal(&k_buf->writable); // 通知有可写空间
    }
}

// 判断缓冲区是否为空
uint8_t kernel_buffer_is_empty(struct kernel_buffer *k_buf) {
    return (k_buf->read_pos == k_buf->write_pos && !k_buf->is_full);
}
