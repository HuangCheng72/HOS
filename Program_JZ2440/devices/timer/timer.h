//
// Created by huangcheng on 2024/7/13.
//

#ifndef HOS_TIMER_H
#define HOS_TIMER_H

#include "../../lib/lib_kernel/lib_kernel.h"

// timer初始化函数
void init_timer();
// timer退出函数
void exit_timer();
// timer中断处理函数
void interrupt_handler_timer();

// ticks是内核自中断开启以来总共的嘀嗒数
extern uint32_t total_ticks;

#endif //HOS_TIMER_H
