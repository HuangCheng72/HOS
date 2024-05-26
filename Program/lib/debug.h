//
// Created by huangcheng on 2024/5/26.
//

#ifndef HOS_DEBUG_C_H
#define HOS_DEBUG_C_H

/***************************  __VA_ARGS__  *******************************
 * __VA_ARGS__ 是预处理器所支持的专用标识符。
 * 代表所有与省略号相对应的参数。
 * "..."表示定义的宏其参数可变。
 ***********************************************************************/

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
