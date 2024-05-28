//
// Created by huangcheng on 2024/5/27.
//

#ifndef HOS_INTERRUPT_H
#define HOS_INTERRUPT_H

// 用枚举定义两个中断状态量（用常量也行，不过枚举的在编程习惯上更好）
enum intr_status {      // 中断状态
    INTR_OFF = 0,       // 中断关闭
    INTR_ON             // 中断打开
};

enum intr_status intr_get_status(void);
enum intr_status intr_set_status (enum intr_status);
enum intr_status intr_enable (void);
enum intr_status intr_disable (void);

#endif //HOS_INTERRUPT_H