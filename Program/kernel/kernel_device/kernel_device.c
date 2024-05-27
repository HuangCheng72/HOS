//
// Created by huangcheng on 2024/5/27.
//

#include "kernel_device.h"
#include "pit8253/pit8253.h"

void init_device() {
    // 初始化内核必需的设备
    init_pit8253();
}