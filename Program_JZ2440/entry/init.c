//
// Created by huangcheng on 2024/7/21.
//

// 这个文件主要是配合head.asm对板子进行初始化，可以用SDRAM，不然加载不了内核
// 初始内存就4KB，只允许用4KB，要先完成这些设置，才能使用SDRAM
// 初始这块内存就是所谓的steppingstone

#include "../lib/type.h"

// 看门狗寄存器地址
#define WTCON   (*(volatile uint32_t *)0x53000000)
#define WTDAT   (*(volatile uint32_t *)0x53000004)
#define WTCNT   (*(volatile uint32_t *)0x53000008)

// 关掉看门狗，不然看门狗会让板子直接重启
void disable_watch_dog(void) __attribute__((section(".text.startup")));
void disable_watch_dog(void) {
    WTCON = 0;  // 关闭看门狗，往这个寄存器写0即可
}

// 设置存储控制器，才能使用SDRAM
void memsetup(void) __attribute__((section(".text.startup")));
void memsetup(void) {
    volatile uint32_t *p = (volatile uint32_t *)(0x48000000);

    // 存储控制器设置
    p[0] = 0x22011110;     // BWSCON
    p[1] = 0x00000700;     // BANKCON0
    p[2] = 0x00000700;     // BANKCON1
    p[3] = 0x00000700;     // BANKCON2
    p[4] = 0x00000700;     // BANKCON3
    p[5] = 0x00000700;     // BANKCON4
    p[6] = 0x00000700;     // BANKCON5
    p[7] = 0x00018005;     // BANKCON6
    p[8] = 0x00018005;     // BANKCON7

    /* 韦东山老师的源代码里面给的刷新率数据是：
     * HCLK=12MHz:  0x008C07A3,
     * HCLK=100MHz: 0x008C04F4
     * 他用的是100MHz，我就不改了
     */
    p[9]  = 0x008C04F4;
    p[10] = 0x000000B1;     // BANKSIZE
    p[11] = 0x00000030;     // MRSRB6
    p[12] = 0x00000030;     // MRSRB7
}

// 把4KB的这部分资源复制到SDRAM去
void copy_steppingstone_to_sdram(void) __attribute__((section(".text.startup")));
void copy_steppingstone_to_sdram(void) {
    uint32_t *pdwSrc  = (uint32_t *)0;
    uint32_t *pdwDest = (uint32_t *)0x30000000;

    while (pdwSrc < (uint32_t *)0x1000) {
        *pdwDest = *pdwSrc;
        pdwDest++;
        pdwSrc++;
    }
}
