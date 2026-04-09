//
// Created by huangcheng on 2025/3/10.
//

#include "ccu.h"
#include "ccu_datasheet.h"

#include "../../kernel/kernel_device/kernel_device.h"

// 最小可用 CCU 初始化：
// 1) PIO 时钟门控（GPIO）
// 2) UART0 APB2 时钟源/分频
// 3) UART0 总线门控 + 复位释放

void ccu_init(void) {

    static uint8_t inited = 0;
    if (inited) {
        return;
    }

    // 1) GPIO 相关：打开 PIO 总线时钟门控
    BUS_CLK_GATING_REG2_t *bus_clk_gating_reg2 = (BUS_CLK_GATING_REG2_t *)(CCU_BASE_ADDR + BUS_CLK_GATING_REG2_OFFSET);
    bus_clk_gating_reg2->PIO_GATING = 1;

    // 2) UART0 相关：APB2 时钟设置为 OSC24M，不做额外预分频，M=0 表示除以 1
    APB2_CFG_REG_t *apb2_cfg = (APB2_CFG_REG_t *)(CCU_BASE_ADDR + APB2_CFG_REG_OFFSET);
    apb2_cfg->APB2_CLK_SRC_SEL = 1;
    apb2_cfg->CLK_RAT_N = 0;
    apb2_cfg->CLK_RAT_M = 0;

    // 3) UART0 总线时钟门控 + 释放 UART0 复位
    BUS_CLK_GATING_REG3_t *bus_clk_gating_reg3 = (BUS_CLK_GATING_REG3_t *)(CCU_BASE_ADDR + BUS_CLK_GATING_REG3_OFFSET);
    bus_clk_gating_reg3->UART0_GATING = 1;

    BUS_SOFT_RST_REG4_t *bus_soft_rst_reg4 = (BUS_SOFT_RST_REG4_t *)(CCU_BASE_ADDR + BUS_SOFT_RST_REG4_OFFSET);
    bus_soft_rst_reg4->UART0_RST = 1;

    inited = 1;
}
