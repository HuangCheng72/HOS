//
// Created by huangcheng on 2024/5/27.
//

#ifndef HOS_PIT8253_H
#define HOS_PIT8253_H

#include "../../kernel/kernel_device/kernel_device.h"

// pit8253初始化函数
void init_pit8253();
// pit8253退出函数
void exit_pit8253();
// pit8253中断处理函数
void interrupt_handler_pit8253();

#endif //HOS_PIT8253_H
