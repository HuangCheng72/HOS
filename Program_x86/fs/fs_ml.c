//
// Created by huangcheng on 2024/6/20.
//

#include "fs_int.h"
#include "../devices/ramdisk/ramdisk.h"

// 长期驻留在内存中的超级块
SuperBlock *SBlk = (SuperBlock *)0x40000;
// 因为以512个字节为基本操作单位，所以文件系统缓冲区大小设计为512个字节
uint8_t *fs_buffer = (uint8_t *)0x42000;

// 这是和ramdisk约定的通信参数格式
uint32_t args[3];

// 参数1是线性地址
// 参数2是数据长度
// 参数3是数据指针

// 扇区大小，定死
#define SECTOR_SIZE 512

// 读入一个扇区数据到文件系统缓冲区（读入前清空缓冲区）
void read_sector(uint32_t sector_idx) {
    // 清空文件系统缓冲区
    memset(fs_buffer, 0, 512);
    // 制作参数
    args[0] = sector_idx * SECTOR_SIZE;
    args[1] = SECTOR_SIZE;
    args[2] = (uint32_t)fs_buffer;

    // 读取数据
    ramdisk_read((char *)args, sizeof(args));

    // 清理参数
    args[0] = 0;
    args[1] = 0;
    args[2] = 0;
}

// 把文件系统缓冲区的信息写出到扇区（写出后清空缓冲区）
void write_sector(uint32_t sector_idx) {
    // 制作参数
    args[0] = sector_idx * SECTOR_SIZE;
    args[1] = SECTOR_SIZE;
    args[2] = (uint32_t)fs_buffer;

    // 读取数据
    ramdisk_write((char *)args, sizeof(args));

    // 清理参数
    args[0] = 0;
    args[1] = 0;
    args[2] = 0;

    // 清空文件系统缓冲区
    memset(fs_buffer, 0, 512);
}
