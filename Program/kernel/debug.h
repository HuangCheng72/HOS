//
// Created by huangcheng on 2024/5/26.
//

#ifndef HOS_DEBUG_C_H
#define HOS_DEBUG_C_H

#include "kernel_interrupt/kernel_interrupt.h"

/***************************  __VA_ARGS__  *******************************
 * __VA_ARGS__ 是预处理器所支持的专用标识符。
 * 代表所有与省略号相对应的参数。
 * "..."表示定义的宏其参数可变。
 ***********************************************************************/

// 补充说明，断言发生报错的时候，多半逻辑已经不能再执行下去，再执行下去反而要系统崩溃
// 这时候如果中断又来，又调用个中断处理函数，说不大直接系统就崩溃了
// 干脆就直接禁止中断、输出信息，然后空转就行了

// 判断条件：编译时有DEBUG编译标记的时候才启用这个宏
#ifdef DEBUG
#define ASSERT(CONDITION, ...)                                                          \
        do {                                                                            \
            if (!(CONDITION)) {                                                         \
                intr_disable();                                                         \
                put_str("Assertion failed!\n");                                         \
                put_str("filename:"); put_str(__FILE__); put_str("\n");                 \
                put_str("line:0x"); put_int(__LINE__); put_str("\n");                   \
                put_str("function:"); put_str((char*)__func__); put_str("\n");          \
                put_str("condition:"); put_str((char*)#CONDITION); put_str("\n");       \
                if (*#__VA_ARGS__) {                                                    \
                    put_str("message:"); put_str((char*)__VA_ARGS__); put_str("\n");    \
                }                                                                       \
                while(1);                                                               \
            }                                                                           \
        } while(0)
#else
#define ASSERT(CONDITION, ...) ((void)0)    //让编译器认为这条代码完全没用，直接忽略
#endif // 结束DEBUG判断

#endif //HOS_DEBUG_C_H
