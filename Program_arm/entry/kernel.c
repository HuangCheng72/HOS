//
// Created by huangcheng on 2024/6/24.
//

#include "../lib/lib_kernel/lib_kernel.h"
#include "../kernel/kernel_page/kernel_page.h"
#include "../kernel/kernel_task/kernel_task.h"
#include "../kernel/kernel_interrupt/kernel_interrupt.h"
#include "../kernel/kernel_device/kernel_device.h"
#include "../devices/console/console.h"
#include "../devices/rtc_cmos/rtc_cmos.h"
#include "../fs/fs.h"

// 文件系统测试函数
void test_fs();

        void kernel_main(void) {
    // u-boot已经给我们设置了管理模式
    // 一进来就已经是管理模式了

    // 页表初始化
    init_paging();

    // 因为覆盖了整个内存区域，所以还可以用u-boot的栈
    // 到这里再切换也没问题

    // 切换栈指针到虚拟地址，这样就可以顺利使用内核栈了
    switch_sp(0xc0007000);
    // 初始化task
    init_multitasking();
    // 初始化内存管理（本来这里应该从环境里面获取内存大小，我懒，直接写死了算了）
    init_memory(0x8000000);
    // 初始化中断管理和GIC
    init_interrupt();
    // 初始化设备驱动管理
    init_all_devices();

    // 开启IRQ中断
    intr_enable();

    // 允许定时器中断
    enable_gic_irq_interrupt(30);

    // 测试文件系统
    test_fs();

    for(;;);

    // 以防万一，退出时退出所有设备
    exit_all_devices();
}

// 文件系统测试函数
void test_fs() {
    FS_DESC fileDesc;
    FS_STAT fileStat;
    uint8_t readBuffer[512];
    const char *testData = "This is a test string for the file system.";
    int16_t result;

    // 启动文件系统
    result = fs_start();
    if (result != 0) {
        return;
    }

    // 格式化文件系统（完全格式化）
    result = fs_format(FS_FORMAT_FULL);
    if (result != 0) {
        fs_exit();
        return;
    }

    // 打开文件（创建并写入）
    result = fs_open(&fileDesc, "/home/user/testfile.txt");
    if (result < 0) {
        fs_exit();
        return;
    }

    // 写入文件
    result = fs_write(&fileDesc, (uint8_t *)testData, strlen(testData));
    if (result < 0) {
        fs_close(&fileDesc);
        fs_exit();
        return;
    }

    // 关闭文件
    result = fs_close(&fileDesc);
    if (result != 0) {
        fs_exit();
        return;
    }

    console_printf("----------------------------------------------------------------\n");

    // 重新打开/home/user/testfile.txt 文件
    result = fs_open(&fileDesc, "/home/user/testfile.txt");
    if (result < 0) {
        console_printf("Failed to open file: /home/user/testfile.txt\n");
        fs_exit();
        return;
    }
    console_printf("File opened: /home/user/testfile.txt\n");

    // 移动前获取文件信息
    result = fs_info(&fileDesc, &fileStat);
    if (result != 0) {
        console_printf("Failed to get file info: /home/user/testfile.txt\n");
        fs_close(&fileDesc);
        fs_exit();
        return;
    }
    console_printf("Before move. File info - Name: %s, Length: %u, Created: %u, Modified: %u, CRC32: %08x\n",
                   fileStat.fname, fileStat.file_len, fileStat.file_ctime, fileStat.file_mtime, fileStat.file_crc32);

    // 并将其移动到 /home/user/newdir/ 目录
    result = fs_move(&fileDesc, "/home/user/newdir/");
    if (result != 0) {
        console_printf("Failed to move file to directory: /home/user/newdir/\n");
        return;
    }
    console_printf("File moved: /home/user/testfile.txt to /home/user/newdir/\n");

    // 关闭文件
    result = fs_close(&fileDesc);
    if (result != 0) {
        console_printf("Failed to close file: /home/user/testfile.txt\n");
        fs_exit();
        return;
    }
    console_printf("File closed: /home/user/testfile.txt\n");

    // 检查文件是否成功移动

    // 以新路径打开
    result = fs_open(&fileDesc, "/home/user/newdir/testfile.txt");
    if (result != 0) {
        console_printf("%d Failed to opened file: /home/user/newdir/testfile.txt\n", result);
        fs_exit();
        return;
    }
    console_printf("File opened: /home/user/newdir/testfile.txt\n");

    // 读取文件
    result = fs_read(&fileDesc, readBuffer, sizeof(readBuffer) - 1);
    if (result < 0) {
        console_printf("Failed to read from file: /home/user/newdir/testfile.txt\n");
        fs_close(&fileDesc);
        fs_exit();
        return;
    }
    console_printf("Data read from file: %s\n", readBuffer);

    // 获取文件信息
    result = fs_info(&fileDesc, &fileStat);
    if (result != 0) {
        console_printf("Failed to get file info: /home/user/newdir/testfile.txt\n");
        fs_close(&fileDesc);
        fs_exit();
        return;
    }
    console_printf("After move. File info - Name: %s, Length: %u, Created: %u, Modified: %u, CRC32: %08x\n",
                   fileStat.fname, fileStat.file_len, fileStat.file_ctime, fileStat.file_mtime, fileStat.file_crc32);

    // 关闭文件
    result = fs_close(&fileDesc);
    if (result != 0) {
        console_printf("Failed to close file after all: /home/user/newdir/testfile.txt\n");
        fs_exit();
        return;
    }
    console_printf("File closed after all: /home/user/newdir/testfile.txt\n");

    // 退出文件系统
    result = fs_exit();
    if (result != 0) {
        console_printf("Failed to exit file system.\n");
        return;
    }
    console_printf("File system exited.\n");
}
