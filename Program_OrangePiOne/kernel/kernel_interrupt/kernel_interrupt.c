//
// Created by huangcheng on 2024/5/27.
//

#include "kernel_interrupt.h"

// ARM用的是GIC（通用中断控制器）

// GIC Distributor（分发器）的基址（看设备树能看到）
// GIC Distributor 是一个负责管理和分配中断的模块。它将中断分发到对应的CPU接口
#define GIC_DIST_BASE 0x01c81000

// GIC CPU Interface（CPU接口）的基址（看设备树能看到）
// GIC CPU Interface 是一个处理和响应分配到该CPU的中断的模块
#define GIC_CPU_BASE  0x01c82000

// GICD_CTLR：GIC分发器控制寄存器
// 这个寄存器用于启用或禁用GIC分发器。写入1启用GIC分发器，写入0禁用它。
#define GICD_CTLR      (*(volatile uint32_t *)(GIC_DIST_BASE + 0x000))

// GICD_ISENABLER：中断启用寄存器
// 这个寄存器用于启用特定的中断源。每个比特位对应一个中断源，写入1启用相应中断。
#define GICD_ISENABLER ((volatile uint32_t *)(GIC_DIST_BASE + 0x100))

// GICD_ICENABLER：中断禁用用寄存器
// 这个寄存器用于禁用特定的中断源。每个比特位对应一个中断源，写入1禁用相应中断。
#define GICD_ICENABLER ((volatile uint32_t *)(GIC_DIST_BASE + 0x180))

// GICD_IPRIORITYR：中断优先级寄存器
// 这个寄存器用于指定中断的优先级。每个字节对应一个中断源。这个和PMR配套，最低0x0，最高0xff。
#define GICD_IPRIORITYR ((volatile uint8_t *)(GIC_DIST_BASE + 0x400))

// GICD_ITARGETSR：中断目标CPU寄存器
// 这个寄存器用于指定中断的目标CPU。每个中断源有一个目标CPU寄存器，用于确定哪个CPU将处理该中断。
#define GICD_ITARGETSR ((volatile uint8_t *)(GIC_DIST_BASE + 0x800))

// GICD_ICFGR：中断触发方式寄存器
// 这个寄存器用于指定中断触发方式，每个比特位表示一种中断的触发方式，0为电平触发（Level Triggered，保持一定的电平值期间触发），1为边缘触发（Edge Triggered，电平变化触发）。
#define GICD_ICFGR ((volatile uint8_t *)(GIC_DIST_BASE + 0xc00))

// GICC_CTLR：GIC CPU接口控制寄存器
// 这个寄存器用于启用或禁用GIC CPU接口。写入1启用GIC CPU接口，写入0禁用它。
#define GICC_CTLR      (*(volatile uint32_t *)(GIC_CPU_BASE + 0x000))

// GICC_PMR：优先级屏蔽寄存器
// 这个寄存器用于设置中断的优先级屏蔽值。只有优先级数值低于该值的中断才会被处理。优先级数值最高为0xff，最低为0，优先级值高的，优先权低，后被处理。
#define GICC_PMR       (*(volatile uint32_t *)(GIC_CPU_BASE + 0x004))

// GICC_BRP：二进制点寄存器
// 这个寄存器用于设置中断优先级的分辨率。确定优先级中的组优先级和子优先级，设置为0可以直接将指定全部是组优先级（只有一组了）
#define GICC_BRP       (*(volatile uint32_t *)(GIC_CPU_BASE + 0x008))

// GICC_IAR：中断确认寄存器
// 这个寄存器用于读取当前待处理的中断ID。当一个中断发生时，CPU通过读取这个寄存器来获取中断ID。
#define GICC_IAR       (*(volatile uint32_t *)(GIC_CPU_BASE + 0x00C))

// GICC_EOIR：中断结束寄存器
// 这个寄存器用于通知GIC中断处理已经完成。写入中断ID表示中断处理结束。
#define GICC_EOIR      (*(volatile uint32_t *)(GIC_CPU_BASE + 0x010))

// 真正的中断处理逻辑数组（用于分发）
// GICv2架构中，中断ID范围从0到1019，其中ID 0-31为SGI（软件生成中断）和PPI（私有外围设备中断），ID 32-1019为SPI（共享外围设备中断）
// GICv3最多支持多达1020个外部中断（SPI）和多达960个LPI（本地性可编程中断），具体取决于实现
void (*interrupt_handler_functions[1020])(void) = { NULL };


// 统一IRQ中断处理分发器
void irq_interrupt_dispatcher(){
    // 读取中断确认寄存器，知道当前中断号是哪个
    uint32_t interrupt_id = GICC_IAR;

    if (interrupt_id < 1020 && interrupt_handler_functions[interrupt_id]) {
        // 如果中断处理逻辑存在，就执行
        interrupt_handler_functions[interrupt_id]();
    }
    // 通知GIC，中断已经执行完毕
    GICC_EOIR = interrupt_id;
}

// 新的的irq处理函数
extern void irq_handler(void);

// 这个函数主要工作类似于init_idt
// 修改中断向量表中的IRQ处理函数，初始化GIC
void init_interrupt() {
    // 由于我自己懒，加上不太清楚中断向量表的玩法，所以只能在u-boot的中断向量表基础上修改
    // 先读取寄存器，确定基址
    uint32_t vbar;
    asm volatile("mrc p15, 0, %0, c12, c0, 0" : "=r" (vbar));

    // 在分页中，内存部分已经全部一对一映射完毕，可以直接原地址访问
    uint32_t *vector_table = (uint32_t *)vbar;
    // 0xe320f000是NOP
    // 从上面打出来的内存数据可以看到，0x44处是一个NOP，可以替换为我们的IRQ中断处理函数地址
    // 数一下就知道下标是17，计算的话可以0x44 / sizeof(uint32_t)（下标从0起算）
    vector_table[17] = (uint32_t)irq_handler;

    // 然后修改IRQ向量的值，也就是vector_table[6]处
    // 该处原值为：0xe59ff014，翻译成汇编指令是 LDR PC, [PC, #0x14]，也就是加载pc寄存器向高地址偏移0x14（20个字节）处的值
    // 执行指令的时候，arm的CPU会指令预取（arm官方资料说预读取后面两条），pc已经 +8 了，需要注意，所以是从当前位置往后偏移 28个 字节
    // 以最前面的未定义中断（忽略复位，那个不关后面的事）为例，指令位置是0x4，那么0xe59ff014就会直接往后偏移28个字节，直接偏移到0x20处
    // 如果要从IRQ向量到0x44处，那就是(17 - 6) * 4 - 8 = 36（十六进制为0x24）
    vector_table[6] = 0xe59ff024; // LDR PC, [PC, #0x24]

    // 启用GIC分配器
    GICD_CTLR = 1;
    // 启用GIC CPU接口
    GICC_CTLR = 1;
    // 设置 GICC_PMR 寄存器为0xff，表示允许所有优先级的中断
    GICC_PMR = 0xff;
    // 所有中断都在一个组中
    GICC_BRP = 0;

    // 设置所有中断优先级
    // 理论上比较重要的要优先，设置更小
    // 普通外设要设置更大
    // 简化这种情况，0x20以下全部是0（默认值），以上全部是0xff
    for(uint32_t i = 0x20; i < 1020; i++) {
        GICD_IPRIORITYR[i] = 0xff;
    }

    // 设置目标CPU，单核系统只有CPU0
    // 所有中断目标CPU都是CPU0
    // 每一个字节代表一个中断寄存器
    for(uint32_t i = 0; i < 1020; i++) {
        // 0x01代表中断指向CPU0
        GICD_ITARGETSR[i] = 0x01;
    }
    // 所有中断触发方式默认都是水平触发（键盘鼠标这些常见外设很多都是边缘触发，后续可能需要重新配置）

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
bool add_interrupt_handler(uint32_t interrupt_id, void (*handler)(void), uint8_t trigger_mode) {
    if(interrupt_id > 1019 || handler == NULL || trigger_mode > 1) {
        return false;
    }
    if(interrupt_handler_functions[interrupt_id] != NULL) {
        // 该位置已有处理函数，不得再添加
        return false;
    }
    interrupt_handler_functions[interrupt_id] = handler;
    // 然后修改触发方式
    // 找相应的位
    uint32_t reg_idx = interrupt_id / 8;
    uint32_t bit_idx = interrupt_id % 8;

    if(trigger_mode) {
        // 已经排除了大于1的问题，所以这里trigger_mode肯定是1
        // 修改该位为1，用位或
        GICD_ICFGR[reg_idx] |= (1 << bit_idx);
    } else {
        // 已经排除了大于1的问题，所以这里trigger_mode肯定是0
        // 修改该位为0，按位取反然后位与即可
        GICD_ICFGR[reg_idx] &= (~(1 << bit_idx));
    }
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
    uint32_t reg_index = irq / 32;
    uint32_t bit_offset = irq % 32;

    // GICD_ICENABLER: 中断禁用寄存器（写1禁用）
    volatile uint32_t *GICD_ICENABLER_REG = GICD_ICENABLER + reg_index;
    // GICD_ISENABLER: 中断启用寄存器
    volatile uint32_t *GICD_ISENABLER_REG = GICD_ISENABLER + reg_index;

    // 写1到对应位禁用中断（用位或防止干扰其他）
    *GICD_ICENABLER_REG |= (1 << bit_offset);

    // 确保启用寄存器中该位被清除（读取-修改-写入）
    uint32_t enable_reg_value = *GICD_ISENABLER_REG;
    enable_reg_value &= ~(1 << bit_offset);
    *GICD_ISENABLER_REG = enable_reg_value;
}

// 启用GIC指定的中断
void enable_gic_irq_interrupt(uint8_t irq) {
    uint32_t reg_index = irq / 32;
    uint32_t bit_offset = irq % 32;

    // GICD_ISENABLER: 中断启用寄存器（写1启用）
    volatile uint32_t *GICD_ISENABLER_REG = GICD_ISENABLER + reg_index;
    // GICD_ICENABLER: 中断禁用寄存器
    volatile uint32_t *GICD_ICENABLER_REG = GICD_ICENABLER + reg_index;

    // 写1到对应位启用中断（用位或防止干扰其他）
    *GICD_ISENABLER_REG |= (1 << bit_offset);

    // 确保禁用寄存器中该位被清除（读取-修改-写入）
    uint32_t disable_reg_value = *GICD_ICENABLER_REG;
    disable_reg_value &= ~(1 << bit_offset);
    *GICD_ICENABLER_REG = disable_reg_value;
}
