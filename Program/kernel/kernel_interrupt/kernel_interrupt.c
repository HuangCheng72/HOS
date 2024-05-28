//
// Created by huangcheng on 2024/5/27.
//

#include "kernel_interrupt.h"

#include "../../lib/lib.h"

// 这么大费周章的主要原因其实就一个，防止多次开/关中断
// 如果开了中断就不要开了，如果关了中断就不要关了

// 封装了cli
extern void disable_interrupts();
// 封装了sti
extern void enable_interrupts();

// 汇编写的取eflags函数
extern void get_eflags(uint32_t* eflags);

// 使用宏进行第二次封装
#define GET_EFLAGS(var) do { get_eflags(&var); } while (0)

// eflags寄存器中的if位为1（表示中断此时处于开启状态）
#define EFLAGS_IF   0x00000200

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
    uint32_t eflags = 0;
    GET_EFLAGS(eflags);
    return (EFLAGS_IF & eflags) ? INTR_ON : INTR_OFF;
}
