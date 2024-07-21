//
// Created by huangcheng on 2024/5/27.
//

#include "kernel_interrupt.h"

// JZ2440用的是什么我也没搞清楚，看起来好像也不是GIC，或者是已经固定好的GIC
// 连哪个中断号代表什么中断都写好了
// 韦东山老师的例程有32个中断

/*interrupt registes*/
#define SRCPND              (*(volatile unsigned long *)0x4A000000)
#define INTMOD              (*(volatile unsigned long *)0x4A000004)
#define INTMSK              (*(volatile unsigned long *)0x4A000008)
#define PRIORITY            (*(volatile unsigned long *)0x4A00000c)
#define INTPND              (*(volatile unsigned long *)0x4A000010)
#define INTOFFSET           (*(volatile unsigned long *)0x4A000014)
#define SUBSRCPND           (*(volatile unsigned long *)0x4A000018)
#define INTSUBMSK           (*(volatile unsigned long *)0x4A00001c)

#define ISR_EINT0_OFT     0
#define ISR_EINT1_OFT     1
#define ISR_EINT2_OFT     2
#define ISR_EINT3_OFT     3
#define ISR_EINT4_7_OFT   4
#define ISR_EINT8_23_OFT  5
#define ISR_NOTUSED6_OFT  6
#define ISR_BAT_FLT_OFT   7
#define ISR_TICK_OFT      8
#define ISR_WDT_OFT       9
#define ISR_TIMER0_OFT    10
#define ISR_TIMER1_OFT    11
#define ISR_TIMER2_OFT    12
#define ISR_TIMER3_OFT    13
#define ISR_TIMER4_OFT    14
#define ISR_UART2_OFT     15
#define ISR_LCD_OFT       16
#define ISR_DMA0_OFT      17
#define ISR_DMA1_OFT      18
#define ISR_DMA2_OFT      19
#define ISR_DMA3_OFT      20
#define ISR_SDI_OFT       21
#define ISR_SPI0_OFT      22
#define ISR_UART1_OFT     23
#define ISR_NOTUSED24_OFT 24
#define ISR_USBD_OFT      25
#define ISR_USBH_OFT      26
#define ISR_IIC_OFT       27
#define ISR_UART0_OFT     28
#define ISR_SPI1_OFT      29
#define ISR_RTC_OFT       30
#define ISR_ADC_OFT       31


// PENDING BIT
#define BIT_EINT0		(0x1)
#define BIT_EINT1		(0x1<<1)
#define BIT_EINT2		(0x1<<2)
#define BIT_EINT3		(0x1<<3)
#define BIT_EINT4_7		(0x1<<4)
#define BIT_EINT8_23	(0x1<<5)
#define BIT_CAM			(0x1<<6)		// Added for 2440.
#define BIT_BAT_FLT		(0x1<<7)
#define BIT_TICK		(0x1<<8)
#define BIT_WDT_AC97	(0x1<<9)
#define BIT_TIMER0		(0x1<<10)
#define BIT_TIMER1		(0x1<<11)
#define BIT_TIMER2		(0x1<<12)
#define BIT_TIMER3		(0x1<<13)
#define BIT_TIMER4		(0x1<<14)
#define BIT_UART2		(0x1<<15)
#define BIT_LCD			(0x1<<16)
#define BIT_DMA0		(0x1<<17)
#define BIT_DMA1		(0x1<<18)
#define BIT_DMA2		(0x1<<19)
#define BIT_DMA3		(0x1<<20)
#define BIT_SDI			(0x1<<21)
#define BIT_SPI0		(0x1<<22)
#define BIT_UART1		(0x1<<23)
#define BIT_NFCON		(0x1<<24)		// Added for 2440.
#define BIT_USBD		(0x1<<25)
#define BIT_USBH		(0x1<<26)
#define BIT_IIC			(0x1<<27)
#define BIT_UART0		(0x1<<28)
#define BIT_SPI1		(0x1<<29)
#define BIT_RTC			(0x1<<30)
#define BIT_ADC			(0x1<<31)
#define BIT_ALLMSK		(0xffffffff)

#define BIT_SUB_ALLMSK	(0x7fff)
#define BIT_SUB_AC97 	(0x1<<14)
#define BIT_SUB_WDT 	(0x1<<13)
#define BIT_SUB_CAM_S	(0x1<<12)		// Added for 2440.
#define BIT_SUB_CAM_C	(0x1<<11)		// Added for 2440.
#define BIT_SUB_ADC		(0x1<<10)
#define BIT_SUB_TC		(0x1<<9)
#define BIT_SUB_ERR2	(0x1<<8)
#define BIT_SUB_TXD2	(0x1<<7)
#define BIT_SUB_RXD2	(0x1<<6)
#define BIT_SUB_ERR1	(0x1<<5)
#define BIT_SUB_TXD1	(0x1<<4)
#define BIT_SUB_RXD1	(0x1<<3)
#define BIT_SUB_ERR0	(0x1<<2)
#define BIT_SUB_TXD0	(0x1<<1)
#define BIT_SUB_RXD0	(0x1<<0)

/*external interrupt registers*/
#define EINTMASK            (*(volatile unsigned long *)0x560000a4)
#define EINTPEND            (*(volatile unsigned long *)0x560000a8)

// 真正的中断处理逻辑数组（用于分发）
// 具体数量懒得改了，反正就用那么多
void (*interrupt_handler_functions[1020])(void) = { NULL };

// 统一IRQ中断处理分发器
void irq_interrupt_dispatcher(){
    // 读取当前中断号是哪个
    uint32_t interrupt_id = INTOFFSET;

    if (interrupt_id < 1020 && interrupt_handler_functions[interrupt_id]) {
        // 如果中断处理逻辑存在，就执行
        interrupt_handler_functions[interrupt_id]();
    }

    // 清除可能的外部中断
    if (interrupt_id == 4) {
        //EINT4-7合用IRQ4，注意EINTPEND[3:0]保留未用，向这些位写入1可能导致未知结果
        EINTPEND = 1 << 7;
    }
    // 清除当前中断
    SRCPND = 1 << interrupt_id;
    // 我也不知道为什么，这是我从韦东山老师的例程里面抄的
    INTPND = INTPND;
}

// 这个函数主要工作是初始化中断控制器
void init_interrupt() {
    // 这里也是直接从韦东山老师的源码里面抄的

    INTMOD = 0x0;	      // 所有中断都设为IRQ模式
    INTMSK = BIT_ALLMSK;  // 先屏蔽所有中断
}

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

// 添加中断处理函数，参数为中断号，中断处理函数，中断触发方式
bool add_interrupt_handler(uint32_t interrupt_id, void (*handler)(void)) {
    if(interrupt_id > 1019 || handler == NULL) {
        return false;
    }
    if(interrupt_handler_functions[interrupt_id] != NULL) {
        // 该位置已有处理函数，不得再添加
        return false;
    }
    interrupt_handler_functions[interrupt_id] = handler;
    return true;
}

// 删除中断处理函数，参数为中断号
bool remove_interrupt_handler(uint32_t interrupt_id) {
    if(interrupt_id > 1019) {
        return false;
    }
    interrupt_handler_functions[interrupt_id] = NULL;
    return (bool)(interrupt_handler_functions[interrupt_id] == NULL);
}

// 禁用GIC指定的中断
void disable_gic_irq_interrupt(uint8_t irq) {
    // 屏蔽位写1
    INTMSK |= (1 << irq);
}

// 启用GIC指定的中断
void enable_gic_irq_interrupt(uint8_t irq) {
    // 这个做法倒是简单，只要把屏蔽位写0就行
    INTMSK &= (~(1 << irq));
}
