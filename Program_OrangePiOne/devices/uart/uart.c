//
// Created by huangcheng on 2026/4/9.
//

#include "uart.h"
#include "uart_datasheet.h"

#include "../../kernel/kernel_device/kernel_device.h"

#include "../../kernel/kernel_buffer/kernel_buffer.h"

// 本驱动只用于实现UART0的相关操作

void uart_init(void);

void uart_exit(void);

int32_t uart_read(char *data, uint32_t count);

int32_t uart_write(const char *data, uint32_t count);

void uart_interrupt_handler();

// UART0需要一个IRQ中断
static struct driver_irq_descriptor uart_irq[1] = {{
       .irq = 32,              // 根据Allwinner_H3_Datasheet_v1.2.pdf，page 206，UART 0的中断号是32
       .irq_interrupt_handler = uart_interrupt_handler,
       .trigger_mode = 0,
}};

// UART0设备提供的字符设备操作集
struct char_device_operator uart_operator = {
        .read = uart_read,
        .write = uart_write,
        .configure = NULL,
};

// 注册驱动结构体
REGISTER_DRIVER(uart_driver) {
        .driver_name = "uart",
        .init = uart_init,
        .exit = uart_exit,
        .irq_descriptors = uart_irq,
        .irq_count = 1,
        .device_type = 1,                       // uart0视为一个字符设备
        .device_operator = &uart_operator,      // uart0提供字符设备操作集
};

struct kernel_buffer *  uart_buffer = NULL;

// UART初始化函数
void uart_init(void) {
    // UART的具体初始化操作
    // 申请缓冲区，并且在退出函数中释放
}

// UART退出函数
void uart_exit(void) {

}

// UART写出，将字符发送出去，成功返回发出的字符总数，失败返回-1
int32_t uart_write(const char *data, uint32_t count) {
    // 本处实际上是替代print.c里面的put_str
    // 修改后，print.c里面的put_str改为调用uart_write，通过驱动调用发送出去

}

// UART中断处理函数
void uart_interrupt_handler() {
    // UART接收到信息时中断，但是目前没想到该怎么处理
    // 直接通过console打印出去
}
