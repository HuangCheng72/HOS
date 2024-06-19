/*******************************************************************************
* JesFs_ll_HOS_ramdisk.c: 用于 RAM 磁盘虚拟设备的低级适配（适配到HOS的虚拟外存设备RAMDISK的驱动）
*
* JesFs - Jo's Embedded Serial File System
*
* 本文件提供了在基于 RAM 的虚拟设备上运行 JesFs 文件系统的低级实现。
* 通过将必要的 JesFs 函数映射到 RAM 磁盘操作，使文件系统能够在没有物理存储硬件的
* 模拟环境中运行。
*
* 由于底层硬件是虚拟的，因此不需要 tb_tools。这些操作通过 RAM 磁盘驱动完成
* 文件系统直接通过RAMDISK驱动对设备操作。
*
* 说白了这个文件就是为了适配jesfs_ml.c
*
* (C) 2024 huangcheng
* 版本: 1.0 / 2024-06-19
*
*******************************************************************************/

#include "jesfs.h"
#include "jesfs_int.h"

// 不用驱动链表转发了
// 直接用设备接口
#include "../devices/ramdisk/ramdisk.h"

// 当前操作地址
uint32_t current_address = 0;
// 当前操作命令
uint8_t current_command = 0;
// 当前闪存寄存器值
uint8_t status_register = 0x00;

// RAMDISK 读写请求参数
uint32_t args[3];

// 初始化 RAMDISK
int16_t sflash_spi_init(void) {
    return 1;
}

void sflash_spi_close(void) {
    // 对于 RAMDISK 不需要任何操作
}

void sflash_wait_usec(uint32_t usec) {
    // 如果需要可以在这里实现延迟，对于 RAMDISK 可以留空
    (void)usec; // 防止编译器报错
}

void sflash_select(void) {
    // 对于唯一的RAMDISK 不需要任何操作
}

void sflash_deselect(void) {
    // 对于 RAMDISK 不需要任何操作
}

// 更新地址
void update_current_address(const uint8_t *cmd) {
    current_address = 0;
    current_address |= (uint32_t)cmd[1] << 16;
    current_address |= (uint32_t)cmd[2] << 8;
    current_address |= (uint32_t)cmd[3];
}

void handle_command(uint8_t cmd, const uint8_t *buf, uint16_t len) {
    current_command = cmd;
    if (len == 4) {
        update_current_address(buf);
    }

    if (cmd == 0x06) { // CMD_WRITEENABLE
        status_register |= 0x02;
    } else if (cmd == 0x05) { // CMD_STATUSREG
        // 不需要在此处处理（写了不在这里读，不用处理）
    } else if (cmd == 0xB9) { // CMD_DEEPPOWERDOWN
        // 模拟深度睡眠，不需要额外处理
    } else if (cmd == 0xAB) { // CMD_RELEASEDPD
        // 模拟唤醒，不需要额外处理
    } else if (cmd == 0xC7) { // CMD_BULKERASE
        // 模拟设备4MB，擦除4MB，即64个块
        for (uint32_t i = 0; i < 64; i++) {
            erase_block(i);
        }
    } else if (cmd == 0x9F) { // CMD_RDID
        // 读设备ID，不用管
    } else if (cmd == 0x20) {
        // 进行粒度为4KB的擦除操作
        erase_block_4K(current_address);
    }
}

void sflash_spi_read(uint8_t *buf, uint16_t len) {
    if (current_command == 0x05) { // CMD_STATUSREG
        buf[0] = status_register;
    } else if (current_command == 0x9F) {
        // 给RAMDISK写一个识别码
        // 我这里用随便一个，MACRONIX_MANU_TYP_RX，0xc228
        // 主要是觉得这个看着顺眼
        // 其含义是，制造商字节 0xC2，设备类型字节 0x28
        // 密度字节 0x12 表示 4MB（方便测试）
        buf[0] = 0xc2;
        buf[1] = 0x28;
        buf[2] = 0x12;
    } else {
        args[0] = current_address;
        args[1] = len;
        args[2] = (uint32_t)buf;

        ramdisk_read((char *)args, sizeof(uint32_t) * 3);

        // 结束之后清理参数
        args[0] = 0;
        args[1] = 0;
        args[2] = 0;
    }
}

void sflash_spi_write(const uint8_t *buf, uint16_t len) {
    if (len == 1) {
        handle_command(buf[0], buf, len);
    } else if (len == 4) {
        handle_command(buf[0], buf, len);
    } else {
        args[0] = current_address;
        args[1] = len;
        args[2] = (uint32_t)buf;

        ramdisk_write((char *)args, sizeof(uint32_t) * 3);;

        // 结束之后清理参数
        args[0] = 0;
        args[1] = 0;
        args[2] = 0;
    }
}
