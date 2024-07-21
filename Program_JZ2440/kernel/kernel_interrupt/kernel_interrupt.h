//
// Created by huangcheng on 2024/5/27.
//

#ifndef HOS_INTERRUPT_H
#define HOS_INTERRUPT_H

#include "../../lib/lib_kernel/lib_kernel.h"

// FIQ（Fast Interrupt Request）和IRQ（Interrupt Request）是ARM架构中的两种中断类型，它们在优先级和处理方式上有所不同。
// 优先级：
// 1. FIQ：具有更高的优先级。在所有中断中，FIQ被优先处理，这意味着在FIQ处理中，IRQ是被屏蔽的。
// 2. IRQ：优先级低于FIQ。当IRQ处理中，如果有FIQ发生，IRQ会被打断，处理FIQ后再返回处理IRQ。
// 寄存器：
// 1. FIQ：有更多的专用寄存器。在FIQ模式下，ARM处理器有7个专用寄存器（R8到R14），这样在处理中断时，可以减少上下文切换的开销，提高中断处理的速度。
// 2. IRQ：只有两个专用寄存器（R13和R14）。需要保存和恢复更多的通用寄存器，增加了上下文切换的开销。
// 用途：
// 1. FIQ：通常用于时间要求非常严格的中断处理，如高速数据传输、实时控制等。这类中断需要快速响应和处理，以减少延迟。
// 2. IRQ：用于一般的中断处理，如键盘输入、网络请求等。这类中断相对来说对响应时间的要求不那么严格。

// 为了简化问题，系统中所有的中断全都是IRQ，这样就不需要处理FIQ的情况


// 这个函数主要工作是初始化中断控制器
void init_interrupt();

// 用枚举定义两个中断状态量（用常量也行，不过枚举的在编程习惯上更好）
enum intr_status {      // 中断状态
    INTR_OFF = 0,       // 中断关闭
    INTR_ON             // 中断打开
};

enum intr_status intr_get_status(void);
enum intr_status intr_set_status (enum intr_status);
enum intr_status intr_enable (void);
enum intr_status intr_disable (void);

// 添加中断处理函数，参数为中断号，中断处理函数
bool add_interrupt_handler(uint32_t interrupt_id, void (*handler)(void));
// 删除中断处理函数，参数为中断号
bool remove_interrupt_handler(uint32_t interrupt_id);
// 启用指定的中断
void enable_gic_irq_interrupt(uint8_t irq);
// 禁用指定的中断
void disable_gic_irq_interrupt(uint8_t irq);

#endif //HOS_INTERRUPT_H
