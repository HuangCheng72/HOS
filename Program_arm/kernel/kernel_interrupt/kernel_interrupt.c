//
// Created by huangcheng on 2024/5/27.
//

#include "kernel_interrupt.h"

// 这么大费周章的主要原因其实就一个，防止多次开/关中断
// 如果开了中断就不要开了，如果关了中断就不要关了

// 封装了arm的禁止中断指令，类似于x86的cli，在clisti.asm中
extern void disable_interrupts();
// 封装了arm的开启中断指令，类似于x86的sti，在clisti.asm中
extern void enable_interrupts();

// 开中断,并且返回开中断前的状态，即允许中断
enum intr_status intr_enable() {
    enum intr_status old_status;
    if (INTR_ON == intr_get_status()) {
        old_status = INTR_ON;
        return old_status;
    } else {
        old_status = INTR_OFF;
        enable_interrupts();    // 开启中断
        return old_status;
    }
}

// 关中断,并且返回关中断前的状态，即不允许中断
enum intr_status intr_disable() {
    enum intr_status old_status;
    if (INTR_ON == intr_get_status()) {
        old_status = INTR_ON;
        disable_interrupts();   // 关闭中断
        return old_status;
    } else {
        old_status = INTR_OFF;
        return old_status;
    }
}

// 将中断状态设置为status
enum intr_status intr_set_status(enum intr_status status) {
    return status & INTR_ON ? intr_enable() : intr_disable();
}

// 获取当前中断状态
enum intr_status intr_get_status() {
    // arm没有eflags寄存器
    // 是CPSR寄存器
    // 直接读这个寄存器然后判断就行了

    // 在ARM架构中，CPSR寄存器中的I位和F位的作用如下：
    //
    // I位（位7）控制IRQ（普通中断）：
    // 当I位为1时，IRQ中断被屏蔽（禁用）
    // 当I位为0时，IRQ中断被开启（启用）
    //
    // F位（位6）控制FIQ（快速中断）：
    // 当F位为1时，FIQ中断被屏蔽（禁用）
    // 当F位为0时，FIQ中断被开启（启用）

    uint32_t cpsr;
    // 使用内联汇编读取CPSR寄存器
    asm volatile ("mrs %0, cpsr" : "=r" (cpsr) : : "memory");
    // 判断IRQ中断是否开启（I位是IRQ，F位是FIQ）
    return (cpsr & (1 << 7)) ? INTR_OFF : INTR_ON;
}
