#include "../lib/lib_kernel/lib_kernel.h"
#include "kernel_page/kernel_page.h"
#include "kernel_gdt/kernel_gdt.h"
#include "kernel_idt/kernel_idt.h"
#include "kernel_device/kernel_device.h"
#include "kernel_memory/kernel_memory.h"
#include "kernel_task/kernel_task.h"
#include "../devices/console/console.h"
#include "kernel_buffer/kernel_buffer.h"
#include "../user/process/process.h"
#include "../devices/ramdisk/ramdisk.h"

#include "../lib/lib_user/syscall.h"

#include "../fs/fs.h"

// 文件系统测试函数
void test_fs();

void kernel_main(void) {

    uint32_t total_physical_memory = *((uint32_t *)(0xa09));
    // 内存分页初始化
    init_paging();
    // 重新加载gdt
    setup_gdt();
    // 调整ESP，将栈指针切换到高地址（栈大小增加到0x1500）
    switch_esp_virtual_addr();
    // 初始化idt
    init_idt();
    // 先初始化任务管理结构（这样所有任务就能用统一的内存管理逻辑了）
    init_multitasking();
    // 初始化内存管理
    init_memory(total_physical_memory);
    // 初始化所有设备
    init_all_devices();

    // 允许PIC_IRQ0中断，才可以让定时器调度线程
    enable_pic_irq_interrupt(0);
    // 开启全局中断
    intr_enable();

    // 测试文件系统
    test_fs();

    // 进入内核主循环或其它初始化代码
    for(;;) {
    }
    // 退出主循环卸载设备驱动
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
