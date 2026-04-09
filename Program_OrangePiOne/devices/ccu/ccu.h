//
// Created by huangcheng on 2025/3/10.
//

#ifndef HOS_CCU_H
#define HOS_CCU_H

#include "../../lib/lib_kernel/lib_kernel.h"

// CCU是比较重要的设备，需要在所有设备初始化之前就初始化好
// CCU不能挂到驱动结构体上，必须手动初始化

void ccu_init(void);

#endif //HOS_CCU_H
