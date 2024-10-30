//
// Created by huangcheng on 2024/7/13.
//

#include "timer.h"

#include "../../kernel/kernel_device/kernel_device.h"
#include "../../kernel/kernel_task/kernel_task.h"

// 注册宏，定义一个驱动结构体，把驱动结构体实例放到驱动段
REGISTER_DRIVER(timer_driver){
        .driver_name = "timer",
        .init = init_timer,
        .exit = exit_timer,
        .irq = 50,              // 根据Allwinner_H3_Datasheet_v1.2.pdf，page 207，Timer 0的中断号是50
        .trigger_mode = 0,
        .irq_interrupt_handler = interrupt_handler_timer,
};

// 这部分内容来自Allwinner_H3_Datasheet_v1.2.pdf，page 156，Timer Register List

// Allwinner H3 Timer 模块基地址
#define TIMER_BASE_ADDR      0x01C20C00

// Timer 寄存器偏移量
#define TMR_IRQ_EN_REG_OFFSET        0x00    // Timer IRQ 使能寄存器
#define TMR_IRQ_STA_REG_OFFSET       0x04    // Timer 状态寄存器
#define TMR0_CTRL_REG_OFFSET         0x10    // Timer 0 控制寄存器
#define TMR0_INTV_VALUE_REG_OFFSET   0x14    // Timer 0 间隔值寄存器
#define TMR0_CUR_VALUE_REG_OFFSET    0x18    // Timer 0 当前值寄存器
#define TMR1_CTRL_REG_OFFSET         0x20    // Timer 1 控制寄存器
#define TMR1_INTV_VALUE_REG_OFFSET   0x24    // Timer 1 间隔值寄存器
#define TMR1_CUR_VALUE_REG_OFFSET    0x28    // Timer 1 当前值寄存器

// AVS（音视频同步）相关寄存器偏移量
#define AVS_CNT_CTL_REG_OFFSET       0x80    // AVS 控制寄存器
#define AVS_CNT0_REG_OFFSET          0x84    // AVS 计数器 0 寄存器
#define AVS_CNT1_REG_OFFSET          0x88    // AVS 计数器 1 寄存器
#define AVS_CNT_DIV_REG_OFFSET       0x8C    // AVS 分频寄存器

// Watchdog（看门狗）相关寄存器偏移量
#define WDOG0_IRQ_EN_REG_OFFSET      0xA0    // Watchdog 0 中断使能寄存器
#define WDOG0_IRQ_STA_REG_OFFSET     0xA4    // Watchdog 0 状态寄存器
#define WDOG0_CTRL_REG_OFFSET        0xB0    // Watchdog 0 控制寄存器
#define WDOG0_CFG_REG_OFFSET         0xB4    // Watchdog 0 配置寄存器
#define WDOG0_MODE_REG_OFFSET        0xB8    // Watchdog 0 模式寄存器

// 完整寄存器地址计算
#define TMR_IRQ_EN_REG        (TIMER_BASE_ADDR + TMR_IRQ_EN_REG_OFFSET)         // Timer IRQ 使能寄存器
#define TMR_IRQ_STA_REG       (TIMER_BASE_ADDR + TMR_IRQ_STA_REG_OFFSET)        // Timer 状态寄存器
#define TMR0_CTRL_REG         (TIMER_BASE_ADDR + TMR0_CTRL_REG_OFFSET)          // Timer 0 控制寄存器
#define TMR0_INTV_VALUE_REG   (TIMER_BASE_ADDR + TMR0_INTV_VALUE_REG_OFFSET)    // Timer 0 间隔值寄存器
#define TMR0_CUR_VALUE_REG    (TIMER_BASE_ADDR + TMR0_CUR_VALUE_REG_OFFSET)     // Timer 0 当前值寄存器
#define TMR1_CTRL_REG         (TIMER_BASE_ADDR + TMR1_CTRL_REG_OFFSET)          // Timer 1 控制寄存器
#define TMR1_INTV_VALUE_REG   (TIMER_BASE_ADDR + TMR1_INTV_VALUE_REG_OFFSET)    // Timer 1 间隔值寄存器
#define TMR1_CUR_VALUE_REG    (TIMER_BASE_ADDR + TMR1_CUR_VALUE_REG_OFFSET)     // Timer 1 当前值寄存器

#define AVS_CNT_CTL_REG       (TIMER_BASE_ADDR + AVS_CNT_CTL_REG_OFFSET)        // AVS 控制寄存器
#define AVS_CNT0_REG          (TIMER_BASE_ADDR + AVS_CNT0_REG_OFFSET)           // AVS 计数器 0 寄存器
#define AVS_CNT1_REG          (TIMER_BASE_ADDR + AVS_CNT1_REG_OFFSET)           // AVS 计数器 1 寄存器
#define AVS_CNT_DIV_REG       (TIMER_BASE_ADDR + AVS_CNT_DIV_REG_OFFSET)        // AVS 分频寄存器

#define WDOG0_IRQ_EN_REG      (TIMER_BASE_ADDR + WDOG0_IRQ_EN_REG_OFFSET)       // Watchdog 0 中断使能寄存器
#define WDOG0_IRQ_STA_REG     (TIMER_BASE_ADDR + WDOG0_IRQ_STA_REG_OFFSET)      // Watchdog 0 状态寄存器
#define WDOG0_CTRL_REG        (TIMER_BASE_ADDR + WDOG0_CTRL_REG_OFFSET)         // Watchdog 0 控制寄存器
#define WDOG0_CFG_REG         (TIMER_BASE_ADDR + WDOG0_CFG_REG_OFFSET)          // Watchdog 0 配置寄存器
#define WDOG0_MODE_REG        (TIMER_BASE_ADDR + WDOG0_MODE_REG_OFFSET)         // Watchdog 0 模式寄存器

// 以下来自于 Allwinner H3 数据手册，p156的 TMR_IRQ_EN_REG Register

// TMR_IRQ_EN_REG 寄存器位域定义
typedef struct {
    uint32_t TMR0_IRQ_EN     : 1;  // [0] Timer 0 中断使能，0=无效，1=使能 Timer 0 到达中断值时触发中断
    uint32_t TMR1_IRQ_EN     : 1;  // [1] Timer 1 中断使能，0=无效，1=使能 Timer 1 到达中断值时触发中断
    uint32_t RESERVED2_31    : 30; // [2:31] 保留未使用，读写时忽略
} TMR_IRQ_EN_REG_t;

// 以下来自于 Allwinner H3 数据手册，p157的 TMR_IRQ_STA_REG Register

// TMR_IRQ_STA_REG 寄存器位域定义
typedef struct {
    uint32_t TMR0_IRQ_PEND   : 1;  // [0] Timer 0 中断挂起状态，0=无效，1=挂起，表示 Timer 0 达到间隔值
    uint32_t TMR1_IRQ_PEND   : 1;  // [1] Timer 1 中断挂起状态，0=无效，1=挂起，表示 Timer 1 达到间隔值
    uint32_t RESERVED2_31    : 30; // [2:31] 保留未使用，读写时忽略
} TMR_IRQ_STA_REG_t;

// 以下来自于 Allwinner H3 数据手册，p157的 TMR0_CTRL_REG Register

// TMR0_CTRL_REG 寄存器位域定义
typedef struct {
    uint32_t TMR0_EN         : 1;  // [0] Timer 0 启动，0=停止/暂停，1=启动
    uint32_t TMR0_RELOAD     : 1;  // [1] Timer 0 重载，0=无效，1=重载 Timer 0 间隔值
    uint32_t TMR0_CLK_SRC    : 2;  // [2:3] Timer 0 时钟源，00=内部OSC/N（约32KHz），01=OSC24M，其他保留
    uint32_t TMR0_CLK_PRES   : 3;  // [4:6] Timer 0 时钟预分频器，000=/1，001=/2，010=/4，011=/8，100=/16，101=/32，110=/64，111=/128
    uint32_t TMR0_MODE       : 1;  // [7] Timer 0 模式，0=连续模式，1=单次模式
    uint32_t RESERVED8_31    : 24; // [8:31] 保留未使用，读写时忽略
} TMR0_CTRL_REG_t;

// 以下来自于 Allwinner H3 数据手册，p158的 TMR0_INTV_VALUE_REG Register

// TMR0_INTV_VALUE_REG 寄存器位域定义
typedef struct {
    uint32_t TMR0_INTV_VALUE : 32; // [0:31] Timer 0 间隔值，定义计数的初始值
} TMR0_INTV_VALUE_REG_t;

// 以下来自于 Allwinner H3 数据手册，p158的 TMR0_CUR_VALUE_REG Register

// TMR0_CUR_VALUE_REG 寄存器位域定义
typedef struct {
    uint32_t TMR0_CUR_VALUE  : 32; // [0:31] Timer 0 当前值，32位递减计数器（从间隔值倒计至 0）
} TMR0_CUR_VALUE_REG_t;

// 以下来自于 Allwinner H3 数据手册，p158的 TMR1_CTRL_REG Register

// TMR1_CTRL_REG 寄存器位域定义
typedef struct {
    uint32_t TMR1_EN         : 1;  // [0] Timer 1 启动，0=停止/暂停，1=启动
    uint32_t TMR1_RELOAD     : 1;  // [1] Timer 1 重载，0=无效，1=重载 Timer 1 间隔值
    uint32_t TMR1_CLK_SRC    : 2;  // [2:3] Timer 1 时钟源，00=内部OSC/N（约32KHz），01=OSC24M，其他保留
    uint32_t TMR1_CLK_PRES   : 3;  // [4:6] Timer 1 时钟预分频器，000=/1，001=/2，010=/4，011=/8，100=/16，101=/32，110=/64，111=/128
    uint32_t TMR1_MODE       : 1;  // [7] Timer 1 模式，0=连续模式，1=单次模式
    uint32_t RESERVED8_31    : 24; // [8:31] 保留未使用，读写时忽略
} TMR1_CTRL_REG_t;

// 以下来自于 Allwinner H3 数据手册，p159的 TMR1_INTV_VALUE_REG Register

// TMR1_INTV_VALUE_REG 寄存器位域定义
typedef struct {
    uint32_t TMR1_INTV_VALUE : 32; // [0:31] Timer 1 间隔值，定义计数的初始值
} TMR1_INTV_VALUE_REG_t;

// 以下来自于 Allwinner H3 数据手册，p159的 TMR1_CUR_VALUE_REG Register

// TMR1_CUR_VALUE_REG 寄存器位域定义
typedef struct {
    uint32_t TMR1_CUR_VALUE  : 32; // [0:31] Timer 1 当前值，32位递减计数器（从间隔值倒计至 0）
} TMR1_CUR_VALUE_REG_t;

// 以下来自于 Allwinner H3 数据手册，p160的 AVS_CNT_CTL_REG Register

// AVS_CNT_CTL_REG 寄存器位域定义
typedef struct {
    uint32_t AVS_CNT0_EN     : 1;  // [0] 音视频同步计数器 0 启动/禁用，0=禁用，1=启用
    uint32_t AVS_CNT1_EN     : 1;  // [1] 音视频同步计数器 1 启动/禁用，0=禁用，1=启用
    uint32_t RESERVED2_6     : 5;  // [2:6] 保留未使用
    uint32_t AVS_CNT0_PS     : 1;  // [7] 音视频同步计数器 0 暂停控制，0=不暂停，1=暂停计数器 0
    uint32_t AVS_CNT1_PS     : 1;  // [8] 音视频同步计数器 1 暂停控制，0=不暂停，1=暂停计数器 1
    uint32_t RESERVED10_31   : 22; // [10:31] 保留未使用
} AVS_CNT_CTL_REG_t;

// 以下来自于 Allwinner H3 数据手册，p160的 AVS_CNT0_REG Register

// AVS_CNT0_REG 寄存器位域定义
typedef struct {
    uint32_t AVS_CNT0        : 32; // [0:31] 音视频同步应用的计数器 0 的高 32 位
} AVS_CNT0_REG_t;

// 以下来自于 Allwinner H3 数据手册，p161的 AVS_CNT1_REG Register

// AVS_CNT1_REG 寄存器位域定义
typedef struct {
    uint32_t AVS_CNT1        : 32; // [0:31] 音视频同步应用的计数器 1 的高 32 位（总 33 位计数器）
} AVS_CNT1_REG_t;

// 以下来自于 Allwinner H3 数据手册，p161的 AVS_CNT_DIV_REG Register

// AVS_CNT_DIV_REG 寄存器位域定义
typedef struct {
    uint32_t AVS_CNT0_D      : 12; // [0:11] AVS 计数器 0 的除数 N0，计算公式：24MHz/ (N0 + 1)
    uint32_t RESERVED12_15   : 4;  // [12:15] 保留未使用
    uint32_t AVS_CNT1_D      : 12; // [16:27] AVS 计数器 1 的除数 N1，计算公式：24MHz/ (N1 + 1)
    uint32_t RESERVED28_31   : 4;  // [28:31] 保留未使用
} AVS_CNT_DIV_REG_t;

// 以下来自于 Allwinner H3 数据手册，p162的 WDOG0_IRQ_EN_REG Register

// WDOG0_IRQ_EN_REG 寄存器位域定义
typedef struct {
    uint32_t WDOG0_IRQ_EN    : 1;  // [0] Watchdog0 中断使能，0=无效，1=使能 Watchdog0 中断
    uint32_t RESERVED1_31    : 31; // [1:31] 保留未使用
} WDOG0_IRQ_EN_REG_t;

// 以下来自于 Allwinner H3 数据手册，p162的 WDOG0_IRQ_STA_REG Register

// WDOG0_IRQ_STA_REG 寄存器位域定义
typedef struct {
    uint32_t WDOG0_IRQ_PEND  : 1;  // [0] Watchdog0 中断挂起，0=无效，1=挂起，表示 Watchdog0 到达间隔值
    uint32_t RESERVED1_31    : 31; // [1:31] 保留未使用
} WDOG0_IRQ_STA_REG_t;

// 以下来自于 Allwinner H3 数据手册，p162的 WDOG0_CTRL_REG Register

// WDOG0_CTRL_REG 寄存器位域定义
typedef struct {
    uint32_t WDOG0_RSTART    : 1;  // [0] Watchdog0 重启，0=无效，1=重启 Watchdog0
    uint32_t RESERVED1_11    : 11; // [1:11] 保留未使用
    uint32_t WDOG0_KEY_FIELD : 20; // [12:31] Watchdog0 密钥字段，需写入 0xA57 以启用写入操作
} WDOG0_CTRL_REG_t;

// timer初始化函数
void init_timer() {

    // 禁用 timer0 和 timer1
    *(volatile uint32_t*)TMR0_CTRL_REG = 0;
    *(volatile uint32_t*)TMR1_CTRL_REG = 0;

    // 初始化 timer1，作为10ms范围以内高精度计时的定时器，但是不触发中断

    TMR1_CTRL_REG_t *timer1_ctrl = (TMR1_CTRL_REG_t *)TMR1_CTRL_REG;

    timer1_ctrl->TMR1_RELOAD = 0;           // 启用重装载（因为是用于计时，每次都不一样，所以不可能重装载）
    timer1_ctrl->TMR1_CLK_SRC = 0;          // 设置时钟源为 24 MHz源
    timer1_ctrl->TMR1_CLK_PRES = 0;         // 不进行预分频
    timer1_ctrl->TMR1_MODE = 1;             // 设置为单次模式（因为是用于计时，每次都不一样，所以不可能设置为连续模式）

    // 设置 timer0 的初始计数值为 10ms 间隔，使用 32kHz 时钟源
    TMR0_CTRL_REG_t *timer0_ctrl = (TMR0_CTRL_REG_t *)TMR0_CTRL_REG;
    timer0_ctrl->TMR0_EN = 0;               // 停止 timer0
    timer0_ctrl->TMR0_RELOAD = 1;           // 启用重装载
    timer0_ctrl->TMR0_CLK_SRC = 0;          // 设置时钟源为内部 32kHz
    timer0_ctrl->TMR0_CLK_PRES = 0;         // 不进行预分频
    timer0_ctrl->TMR0_MODE = 0;             // 设置为连续模式

    // 我要的中断频率是100Hz，也就是一秒钟中断100次
    // 那么计数值就是 32 * 1000 / 100 = 320

    *(volatile uint32_t*)TMR0_INTV_VALUE_REG = 320;  // 10ms 间隔（32kHz 时钟源）

    // 启用 timer0 的中断
    TMR_IRQ_EN_REG_t* irq_en_reg = (TMR_IRQ_EN_REG_t*)TMR_IRQ_EN_REG;
    irq_en_reg->TMR0_IRQ_EN = 1;

    // 启动 timer0
    timer0_ctrl->TMR0_EN = 1;
}

// timer退出函数
void exit_timer() {
    // 禁用 timer0 和 timer1
    TMR0_CTRL_REG_t* timer0_ctrl = (TMR0_CTRL_REG_t*)TMR0_CTRL_REG;
    TMR1_CTRL_REG_t* timer1_ctrl = (TMR1_CTRL_REG_t*)TMR1_CTRL_REG;

    timer0_ctrl->TMR0_EN = 0;  // 禁用 timer0
    timer1_ctrl->TMR1_EN = 0;  // 禁用 timer1

    // 禁用所有 Timer 中断
    TMR_IRQ_EN_REG_t* irq_en_reg = (TMR_IRQ_EN_REG_t*)TMR_IRQ_EN_REG;
    irq_en_reg->TMR0_IRQ_EN = 0;
    irq_en_reg->TMR1_IRQ_EN = 0;
}

// 初始化
uint32_t total_ticks = 0;

// timer中断处理函数
void interrupt_handler_timer() {
    // 逻辑代码
    struct task* cur_task = running_task();
    cur_task->elapsed_ticks++;	  // 记录此线程占用的cpu时间嘀嗒数

    total_ticks++;                // 总时长增加

    if (cur_task->ticks == 0) {	  // 若任务时间片用完就开始调度新的任务上cpu
        task_schedule();
    } else {				  // 将当前任务的时间片-1
        cur_task->ticks--;
    }

    // 清除 Timer 0 中断挂起状态
    TMR_IRQ_STA_REG_t* irq_status = (TMR_IRQ_STA_REG_t*)TMR_IRQ_STA_REG;
    irq_status->TMR0_IRQ_PEND = 1;
}

// 设置 timer1 延时（微秒）
void set_timer1_delay_us(uint32_t us) {
    // 计算倒计时的计数值
    // 如果是1us，那么就是每秒频率10^6次，也就是1M
    // 那么每us计时就是 24MHz / 1MHz = 24
    uint32_t count_value = us * 24;

    // 停止 timer1 并设置倒计时值
    TMR1_CTRL_REG_t *timer1_ctrl = (TMR1_CTRL_REG_t *)TMR1_CTRL_REG;
    timer1_ctrl->TMR1_EN = 0;            // 停止计数

    *(volatile uint32_t *)TMR1_INTV_VALUE_REG = count_value;
    *(volatile uint32_t *)TMR1_CUR_VALUE_REG = 0;

    // 其他参数已经在初始化中配置好，这里不要配置
    timer1_ctrl->TMR1_EN = 1;            // 启动 timer1
}

// 检查 timer1 是否完成倒计时
bool is_timer1_done() {
    return (*(volatile uint32_t *)TMR1_CUR_VALUE_REG == 0);
}
