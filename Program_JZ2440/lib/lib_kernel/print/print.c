//
// Created by huangcheng on 2024/6/27.
//

#include "print.h"

// 状态寄存器
#define UTRSTAT0            (*(volatile unsigned long *)0x50000010)
// 数据寄存器
#define UTXH0               (*(volatile unsigned char *)0x50000020)

#define TXD0READY   (1 << 2)
#define RXD0READY   (1)

void uart_send_char(char c) {
    // 等待，直到发送缓冲区中的数据已经全部发送出去
    while (!(UTRSTAT0 & TXD0READY));

    // 向UTXH0寄存器中写入数据，UART即自动将它发送出去
    UTXH0 = c;
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
