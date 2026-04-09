//
// Created by huangcheng on 2026/4/9.
//

#include "uart.h"
#include "uart_datasheet.h"

#include "../../kernel/kernel_device/kernel_device.h"

// 本驱动只用于实现UART0的相关操作

void uart_init(void);

void uart_exit(void);

int32_t uart_read(char *args, uint32_t args_size);

int32_t uart_write(char *args, uint32_t args_size);

void uart_interrupt_handler();

// 注册驱动结构体
REGISTER_DRIVER(uart_driver) {
        .driver_name = "uart",
        .init = uart_init,
        .exit = uart_exit,
        .read = NULL,               // 读入的内容都在数据缓冲区内，直接用device_read读缓冲区就行了
        .write = uart_write,
        .irq = 32,                  // 根据Allwinner_H3_Datasheet_v1.2.pdf，page 207，UART 0的中断号是32
        .irq_interrupt_handler = uart_interrupt_handler,
        .need_command_buffer = 0,   // 明确，命令缓冲区是用于写出的，这里用不到，直接写出了
        .need_data_buffer = 1,      // 明确，数据缓冲区是用于读入的，这里用到，应当申请
};

// UART初始化函数
void uart_init(void) {

}

// UART退出函数
void uart_exit(void) {

}

// UART写出，将字符发送出去，成功返回发出的字符总数，失败返回-1
int32_t uart_write(char *args, uint32_t args_size) {
    // 本处实际上是替代print.c里面的put_str

}

// UART中断处理函数
void uart_interrupt_handler() {
    // UART接收到信息时中断
    // 如果需要缓冲区的话，就用uart_driver.data_buffer
    // 目前设想是把接收到的信息先用console_printf打印出来
    // 后面在console里面完善相关命令操作
}
