//
// Created by huangcheng on 2024/10/30.
//

#ifndef HOS_KERNEL_DELAY_H
#define HOS_KERNEL_DELAY_H

#include "../../lib/lib_kernel/lib_kernel.h"

// 对于8ms（8000us）以下的延时，该函数直接使用 timer1 的 us 级延时功能，并通过轮询等待直到时间到达目标。该函数只能用于8ms以下的延时。
void kernel_delay(uint32_t us);

#endif //HOS_KERNEL_DELAY_H
