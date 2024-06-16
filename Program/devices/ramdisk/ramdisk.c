//
// Created by Administrator on 2024/6/16.
//

#include "ramdisk.h"
#include "../../kernel/kernel_device/kernel_device.h"
#include "../console/console.h"
#include "../../kernel/kernel_task/kernel_task.h"

// 注册驱动结构体
REGISTER_DRIVER(ramdisk_driver) {
        .driver_name = "ramdisk",
        .init = NULL,
        .exit = NULL,
        .irq = -1, // 没有中断处理
        .irq_interrupt_handler = NULL,
        .need_data_buffer = 1,
        .need_command_buffer = 1,
        .data_buffer = NULL,
        .command_buffer = NULL,
        .driver_task_function = ramdisk_task_function,
        .driver_task_function_args = NULL,
};

// 仅限此处使用，所以要用static，限制作用域在此文件中
static uint8_t *ramdisk = (uint8_t *)0xA0000000; // 定义RAM Disk的存储空间

struct ramdisk_io_request {
    uint32_t sector;
    uint32_t size;
    char data[SECTOR_SIZE];
};

int32_t ramdisk_read(int sector, void *buffer, uint32_t size) {
    if (sector * SECTOR_SIZE + size > RAMDISK_SIZE) {
        return -1; // 超出范围
    }
    memcpy(buffer, ramdisk + sector * SECTOR_SIZE, size);
    return size;
}

int32_t ramdisk_write(int sector, const void *buffer, uint32_t size) {
    if (sector * SECTOR_SIZE + size > RAMDISK_SIZE) {
        return -1; // 超出范围
    }
    memcpy(ramdisk + sector * SECTOR_SIZE, buffer, size);
    return size;
}

void ramdisk_task_function() {
    uint32_t sector, size, command;
    uint32_t bytes_processed;
    char buffer[1024];

    for(;;) {
        // 每次循环先清空
        sector = 0;
        size = 0;
        command = 0;
        // 先处理命令缓冲区中的数据

        // 读取扇区、大小和命令标志，这是事先约定好的命令数据规格，每个操作都必备的
        kernel_buffer_read(ramdisk_driver.command_buffer, (char *)&sector, sizeof(uint32_t));
        kernel_buffer_read(ramdisk_driver.command_buffer, (char *)&size, sizeof(uint32_t));
        kernel_buffer_read(ramdisk_driver.command_buffer, (char *)&command, sizeof(uint32_t));

        // 根据命令执行相应操作
        switch (command) {
            case WRITE_CMD:
                // 处理写请求

                // 写入数据
                bytes_processed = 0;
                while (bytes_processed < size) {
                    uint32_t chunk_size = (size - bytes_processed > 1024) ? 1024 : (size - bytes_processed);
                    // 命令缓冲区中剩余的数据就是要写入的实际数据
                    kernel_buffer_read(ramdisk_driver.command_buffer, buffer, chunk_size);

                    ramdisk_write(sector + (bytes_processed / SECTOR_SIZE), buffer, chunk_size);
                    bytes_processed += chunk_size;
                }
                break;

            case READ_CMD:
                // 处理读请求

                bytes_processed = 0;
                while (bytes_processed < size) {
                    uint32_t chunk_size = (size - bytes_processed > 1024) ? 1024 : (size - bytes_processed);
                    ramdisk_read(sector + (bytes_processed / SECTOR_SIZE), buffer, chunk_size);
                    // 读出来的数据写到数据缓冲区
                    kernel_buffer_write(ramdisk_driver.data_buffer, buffer, chunk_size);
                    bytes_processed += chunk_size;
                }
                break;

            case DELETE_CMD:
                // 处理删除请求
                memset(ramdisk + sector * SECTOR_SIZE, 0, size);
                break;

            default:
                // 处理未知命令
                console_put_str("\nread_pos : ");
                console_put_int(ramdisk_driver.command_buffer->read_pos);
                console_put_str("\n");
                console_put_int(command);
                console_put_str("        Unknown command received!\n");

                break;
        }
    }
}
