//
// Created by huangcheng on 2024/6/27.
//

#include "print.h"

// 这是根据环境写的（环境变量里面UART0映射范围就在这两个内存地址上）
#define UART0_BASE 0x9000000
// UARTDR (Data Register)：发送和接收数据。
#define UARTDR     (*(volatile unsigned int *)(UART0_BASE + 0x000))
// UARTFR (Flag Register)：包含各种状态标志，包括串口是否忙碌。
#define UARTFR     (*(volatile unsigned int *)(UART0_BASE + 0x018))

void uart_send_char(char c) {
    // 等待串口不忙碌的时候，才能发送消息
    while (UARTFR & (1 << 5)) {
        // 这里采用轮询的方法，实际上可以做一些别的事情
    }
    // 写一个字符到数据寄存器映射的内存地址
    UARTDR = c;
}

// 基于uart_send_char，重写老三样

// 将一个字符打印在光标处
void put_char(char c) {
    uart_send_char(c);
}

void put_str(const char *str) {
    while (*str) {
        uart_send_char(*str++);
    }
}

void put_int(int n) {
    char hex_chars[] = "0123456789ABCDEF";
    for (int i = 28; i >= 0; i -= 4) {
        uart_send_char(hex_chars[(n >> i) & 0xF]);
    }
}
