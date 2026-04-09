//
// Created by huangcheng on 2026/4/9.
//

#ifndef HOS_GPIO_DATASHEET_H
#define HOS_GPIO_DATASHEET_H

#include "../../lib/lib_kernel/lib_kernel.h"

// 这部分内容来自 Allwinner_H3_Datasheet_v1.2.pdf，page 316-344，PIO Register List / Register Description

// Allwinner H3 PIO（CPU-PORT）模块基地址
#define PIO_BASE_ADDR                    0x01C20800  // PIO（CPU-PORT）基地址

// Port 编号（供 Pn_* 通用偏移公式使用）
#define PIO_PORT_A_INDEX                 0
#define PIO_PORT_C_INDEX                 1
#define PIO_PORT_D_INDEX                 2
#define PIO_PORT_E_INDEX                 3
#define PIO_PORT_F_INDEX                 4
#define PIO_PORT_G_INDEX                 5
#define PIO_PORT_L_INDEX                 6

// ========================= 通用 Port 寄存器偏移 =========================
// 每个 Port 寄存器块步长：0x24
#define PIO_PORT_STRIDE                  0x24

#define Pn_CFG0_REG_OFFSET(n)            ((n) * PIO_PORT_STRIDE + 0x00)  // Port n 配置寄存器 0
#define Pn_CFG1_REG_OFFSET(n)            ((n) * PIO_PORT_STRIDE + 0x04)  // Port n 配置寄存器 1
#define Pn_CFG2_REG_OFFSET(n)            ((n) * PIO_PORT_STRIDE + 0x08)  // Port n 配置寄存器 2
#define Pn_CFG3_REG_OFFSET(n)            ((n) * PIO_PORT_STRIDE + 0x0C)  // Port n 配置寄存器 3
#define Pn_DAT_REG_OFFSET(n)             ((n) * PIO_PORT_STRIDE + 0x10)  // Port n 数据寄存器
#define Pn_DRV0_REG_OFFSET(n)            ((n) * PIO_PORT_STRIDE + 0x14)  // Port n 驱动能力寄存器 0
#define Pn_DRV1_REG_OFFSET(n)            ((n) * PIO_PORT_STRIDE + 0x18)  // Port n 驱动能力寄存器 1
#define Pn_PUL0_REG_OFFSET(n)            ((n) * PIO_PORT_STRIDE + 0x1C)  // Port n 上下拉寄存器 0
#define Pn_PUL1_REG_OFFSET(n)            ((n) * PIO_PORT_STRIDE + 0x20)  // Port n 上下拉寄存器 1

// ========================= PA 外部中断寄存器偏移 =========================
#define PA_EINT_CFG0_REG_OFFSET          0x200  // PA 外部中断配置寄存器 0
#define PA_EINT_CFG1_REG_OFFSET          0x204  // PA 外部中断配置寄存器 1
#define PA_EINT_CFG2_REG_OFFSET          0x208  // PA 外部中断配置寄存器 2
#define PA_EINT_CFG3_REG_OFFSET          0x20C  // PA 外部中断配置寄存器 3
#define PA_EINT_CTL_REG_OFFSET           0x210  // PA 外部中断控制寄存器
#define PA_EINT_STATUS_REG_OFFSET        0x214  // PA 外部中断状态寄存器
#define PA_EINT_DEB_REG_OFFSET           0x218  // PA 外部中断去抖寄存器

// ========================= PG 外部中断寄存器偏移 =========================
#define PG_EINT_CFG0_REG_OFFSET          0x220  // PG 外部中断配置寄存器 0
#define PG_EINT_CFG1_REG_OFFSET          0x224  // PG 外部中断配置寄存器 1
#define PG_EINT_CFG2_REG_OFFSET          0x228  // PG 外部中断配置寄存器 2
#define PG_EINT_CFG3_REG_OFFSET          0x22C  // PG 外部中断配置寄存器 3
#define PG_EINT_CTL_REG_OFFSET           0x230  // PG 外部中断控制寄存器
#define PG_EINT_STATUS_REG_OFFSET        0x234  // PG 外部中断状态寄存器
#define PG_EINT_DEB_REG_OFFSET           0x238  // PG 外部中断去抖寄存器


// 以下来自于 Allwinner H3 数据手册，p317的 PA_CFG0_REG Register

// PA 配置寄存器 0
typedef struct {
    uint32_t PA0_SELECT        : 3;  // [0:2]   PA0 功能选择 (000:Input, 001:Output, 010:UART2_TX, 011:JTAG_MS, 110:PA_EINT0, 111:IO Disable)
    uint32_t RESERVED3         : 1;  // [3]     保留
    uint32_t PA1_SELECT        : 3;  // [4:6]   PA1 功能选择 (000:Input, 001:Output, 010:UART2_RX, 011:JTAG_CK, 110:PA_EINT1, 111:IO Disable)
    uint32_t RESERVED7         : 1;  // [7]     保留
    uint32_t PA2_SELECT        : 3;  // [8:10]  PA2 功能选择 (000:Input, 001:Output, 010:UART2_RTS, 011:JTAG_DO, 110:PA_EINT2, 111:IO Disable)
    uint32_t RESERVED11        : 1;  // [11]    保留
    uint32_t PA3_SELECT        : 3;  // [12:14] PA3 功能选择 (000:Input, 001:Output, 010:UART2_CTS, 011:JTAG_DI, 110:PA_EINT3, 111:IO Disable)
    uint32_t RESERVED15        : 1;  // [15]    保留
    uint32_t PA4_SELECT        : 3;  // [16:18] PA4 功能选择 (000:Input, 001:Output, 010:UART0_TX, 110:PA_EINT4, 111:IO Disable)
    uint32_t RESERVED19        : 1;  // [19]    保留
    uint32_t PA5_SELECT        : 3;  // [20:22] PA5 功能选择 (000:Input, 001:Output, 010:UART0_RX, 011:PWM0, 110:PA_EINT5, 111:IO Disable)
    uint32_t RESERVED23        : 1;  // [23]    保留
    uint32_t PA6_SELECT        : 3;  // [24:26] PA6 功能选择 (000:Input, 001:Output, 010:SIM_PWREN, 110:PA_EINT6, 111:IO Disable)
    uint32_t RESERVED27        : 1;  // [27]    保留
    uint32_t PA7_SELECT        : 3;  // [28:30] PA7 功能选择 (000:Input, 001:Output, 010:SIM_CLK, 110:PA_EINT7, 111:IO Disable)
    uint32_t RESERVED31        : 1;  // [31]    保留
} PA_CFG0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p318的 PA_CFG1_REG Register

// PA 配置寄存器 1
typedef struct {
    uint32_t PA8_SELECT        : 3;  // [0:2]   PA8 功能选择  (000:Input, 001:Output, 010:SIM_DATA, 110:PA_EINT8, 111:IO Disable)
    uint32_t RESERVED3         : 1;  // [3]     保留
    uint32_t PA9_SELECT        : 3;  // [4:6]   PA9 功能选择  (000:Input, 001:Output, 010:SIM_RST, 110:PA_EINT9, 111:IO Disable)
    uint32_t RESERVED7         : 1;  // [7]     保留
    uint32_t PA10_SELECT       : 3;  // [8:10]  PA10 功能选择 (000:Input, 001:Output, 010:SIM_DET, 110:PA_EINT10, 111:IO Disable)
    uint32_t RESERVED11        : 1;  // [11]    保留
    uint32_t PA11_SELECT       : 3;  // [12:14] PA11 功能选择 (000:Input, 001:Output, 010:TWI0_SCK, 011:DI_TX, 110:PA_EINT11, 111:IO Disable)
    uint32_t RESERVED15        : 1;  // [15]    保留
    uint32_t PA12_SELECT       : 3;  // [16:18] PA12 功能选择 (000:Input, 001:Output, 010:TWI0_SDA, 011:DI_RX, 110:PA_EINT12, 111:IO Disable)
    uint32_t RESERVED19        : 1;  // [19]    保留
    uint32_t PA13_SELECT       : 3;  // [20:22] PA13 功能选择 (000:Input, 001:Output, 010:SPI1_CS, 011:UART3_TX, 110:PA_EINT13, 111:IO Disable)
    uint32_t RESERVED23        : 1;  // [23]    保留
    uint32_t PA14_SELECT       : 3;  // [24:26] PA14 功能选择 (000:Input, 001:Output, 010:SPI1_CLK, 011:UART3_RX, 110:PA_EINT14, 111:IO Disable)
    uint32_t RESERVED27        : 1;  // [27]    保留
    uint32_t PA15_SELECT       : 3;  // [28:30] PA15 功能选择 (000:Input, 001:Output, 010:SPI1_MOSI, 011:UART3_RTS, 110:PA_EINT15, 111:IO Disable)
    uint32_t RESERVED31        : 1;  // [31]    保留
} PA_CFG1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p319的 PA_CFG2_REG Register

// PA 配置寄存器 2
typedef struct {
    uint32_t PA16_SELECT       : 3;  // [0:2]   PA16 功能选择 (000:Input, 001:Output, 010:SPI1_MISO, 011:UART3_CTS, 110:PA_EINT16, 111:IO Disable)
    uint32_t RESERVED3         : 1;  // [3]     保留
    uint32_t PA17_SELECT       : 3;  // [4:6]   PA17 功能选择 (000:Input, 001:Output, 010:OWA_OUT, 110:PA_EINT17, 111:IO Disable)
    uint32_t RESERVED7         : 1;  // [7]     保留
    uint32_t PA18_SELECT       : 3;  // [8:10]  PA18 功能选择 (000:Input, 001:Output, 010:PCM0_SYNC, 011:TWI1_SCK, 110:PA_EINT18, 111:IO Disable)
    uint32_t RESERVED11        : 1;  // [11]    保留
    uint32_t PA19_SELECT       : 3;  // [12:14] PA19 功能选择 (000:Input, 001:Output, 010:PCM0_CLK, 011:TWI1_SDA, 110:PA_EINT19, 111:IO Disable)
    uint32_t RESERVED15        : 1;  // [15]    保留
    uint32_t PA20_SELECT       : 3;  // [16:18] PA20 功能选择 (000:Input, 001:Output, 010:PCM0_DOUT, 011:SIM_VPPEN, 110:PA_EINT20, 111:IO Disable)
    uint32_t RESERVED19        : 1;  // [19]    保留
    uint32_t PA21_SELECT       : 3;  // [20:22] PA21 功能选择 (000:Input, 001:Output, 010:PCM0_DIN, 011:SIM_VPPPP, 110:PA_EINT21, 111:IO Disable)
    uint32_t RESERVED23_31     : 9;  // [23:31] 保留
} PA_CFG2_REG_t;


// 以下来自于 Allwinner H3 数据手册，p320的 PA_CFG3_REG Register

// PA 配置寄存器 3
typedef struct {
    uint32_t RESERVED0_31      : 32; // [0:31] 保留
} PA_CFG3_REG_t;


// 以下来自于 Allwinner H3 数据手册，p320的 PA_DATA_REG Register

// PA 数据寄存器
typedef struct {
    uint32_t PA_DAT            : 22; // [0:21]  PA 数据位 (输入模式下为引脚状态，输出模式下为软件设置值，复用功能模式下读值未定义)
    uint32_t RESERVED22_31     : 10; // [22:31] 保留
} PA_DATA_REG_t;


// 以下来自于 Allwinner H3 数据手册，p320的 PA_DRV0_REG Register

// PA 驱动能力寄存器 0
typedef struct {
    uint32_t PA0_DRV           : 2;  // [0:1]   PA0 驱动等级选择 (00:Level0, 01:Level1, 10:Level2, 11:Level3)
    uint32_t PA1_DRV           : 2;  // [2:3]   PA1 驱动等级选择
    uint32_t PA2_DRV           : 2;  // [4:5]   PA2 驱动等级选择
    uint32_t PA3_DRV           : 2;  // [6:7]   PA3 驱动等级选择
    uint32_t PA4_DRV           : 2;  // [8:9]   PA4 驱动等级选择
    uint32_t PA5_DRV           : 2;  // [10:11] PA5 驱动等级选择
    uint32_t PA6_DRV           : 2;  // [12:13] PA6 驱动等级选择
    uint32_t PA7_DRV           : 2;  // [14:15] PA7 驱动等级选择
    uint32_t PA8_DRV           : 2;  // [16:17] PA8 驱动等级选择
    uint32_t PA9_DRV           : 2;  // [18:19] PA9 驱动等级选择
    uint32_t PA10_DRV          : 2;  // [20:21] PA10 驱动等级选择
    uint32_t PA11_DRV          : 2;  // [22:23] PA11 驱动等级选择
    uint32_t PA12_DRV          : 2;  // [24:25] PA12 驱动等级选择
    uint32_t PA13_DRV          : 2;  // [26:27] PA13 驱动等级选择
    uint32_t PA14_DRV          : 2;  // [28:29] PA14 驱动等级选择
    uint32_t PA15_DRV          : 2;  // [30:31] PA15 驱动等级选择
} PA_DRV0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p321的 PA_DRV1_REG Register

// PA 驱动能力寄存器 1
typedef struct {
    uint32_t PA16_DRV          : 2;  // [0:1]   PA16 驱动等级选择
    uint32_t PA17_DRV          : 2;  // [2:3]   PA17 驱动等级选择
    uint32_t PA18_DRV          : 2;  // [4:5]   PA18 驱动等级选择
    uint32_t PA19_DRV          : 2;  // [6:7]   PA19 驱动等级选择
    uint32_t PA20_DRV          : 2;  // [8:9]   PA20 驱动等级选择
    uint32_t PA21_DRV          : 2;  // [10:11] PA21 驱动等级选择
    uint32_t RESERVED12_31     : 20; // [12:31] 保留
} PA_DRV1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p321的 PA_PULL0_REG Register

// PA 上下拉寄存器 0
typedef struct {
    uint32_t PA0_PULL          : 2;  // [0:1]   PA0 上下拉选择 (00:Disable, 01:Pull-up, 10:Pull-down, 11:Reserved)
    uint32_t PA1_PULL          : 2;  // [2:3]   PA1 上下拉选择
    uint32_t PA2_PULL          : 2;  // [4:5]   PA2 上下拉选择
    uint32_t PA3_PULL          : 2;  // [6:7]   PA3 上下拉选择
    uint32_t PA4_PULL          : 2;  // [8:9]   PA4 上下拉选择
    uint32_t PA5_PULL          : 2;  // [10:11] PA5 上下拉选择
    uint32_t PA6_PULL          : 2;  // [12:13] PA6 上下拉选择
    uint32_t PA7_PULL          : 2;  // [14:15] PA7 上下拉选择
    uint32_t PA8_PULL          : 2;  // [16:17] PA8 上下拉选择
    uint32_t PA9_PULL          : 2;  // [18:19] PA9 上下拉选择
    uint32_t PA10_PULL         : 2;  // [20:21] PA10 上下拉选择
    uint32_t PA11_PULL         : 2;  // [22:23] PA11 上下拉选择
    uint32_t PA12_PULL         : 2;  // [24:25] PA12 上下拉选择
    uint32_t PA13_PULL         : 2;  // [26:27] PA13 上下拉选择
    uint32_t PA14_PULL         : 2;  // [28:29] PA14 上下拉选择
    uint32_t PA15_PULL         : 2;  // [30:31] PA15 上下拉选择
} PA_PULL0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p321的 PA_PULL1_REG Register

// PA 上下拉寄存器 1
typedef struct {
    uint32_t PA16_PULL         : 2;  // [0:1]   PA16 上下拉选择
    uint32_t PA17_PULL         : 2;  // [2:3]   PA17 上下拉选择
    uint32_t PA18_PULL         : 2;  // [4:5]   PA18 上下拉选择
    uint32_t PA19_PULL         : 2;  // [6:7]   PA19 上下拉选择
    uint32_t PA20_PULL         : 2;  // [8:9]   PA20 上下拉选择
    uint32_t PA21_PULL         : 2;  // [10:11] PA21 上下拉选择
    uint32_t RESERVED12_31     : 20; // [12:31] 保留
} PA_PULL1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p322的 PC_CFG0_REG Register

// PC 配置寄存器 0
typedef struct {
    uint32_t PC0_SELECT        : 3;  // [0:2]   PC0 功能选择 (000:Input, 001:Output, 010:NAND_WE, 011:SPI0_MOSI, 111:IO Disable)
    uint32_t RESERVED3         : 1;  // [3]     保留
    uint32_t PC1_SELECT        : 3;  // [4:6]   PC1 功能选择 (000:Input, 001:Output, 010:NAND_ALE, 011:SPI0_MISO, 111:IO Disable)
    uint32_t RESERVED7         : 1;  // [7]     保留
    uint32_t PC2_SELECT        : 3;  // [8:10]  PC2 功能选择 (000:Input, 001:Output, 010:NAND_CLE, 011:SPI0_CLK, 111:IO Disable)
    uint32_t RESERVED11        : 1;  // [11]    保留
    uint32_t PC3_SELECT        : 3;  // [12:14] PC3 功能选择 (000:Input, 001:Output, 010:NAND_CE1, 011:SPI0_CS, 111:IO Disable)
    uint32_t RESERVED15        : 1;  // [15]    保留
    uint32_t PC4_SELECT        : 3;  // [16:18] PC4 功能选择 (000:Input, 001:Output, 010:NAND_CE0, 111:IO Disable)
    uint32_t RESERVED19        : 1;  // [19]    保留
    uint32_t PC5_SELECT        : 3;  // [20:22] PC5 功能选择 (000:Input, 001:Output, 010:NAND_RE, 011:SDC2_CLK, 111:IO Disable)
    uint32_t RESERVED23        : 1;  // [23]    保留
    uint32_t PC6_SELECT        : 3;  // [24:26] PC6 功能选择 (000:Input, 001:Output, 010:NAND_RB0, 011:SDC2_CMD, 111:IO Disable)
    uint32_t RESERVED27        : 1;  // [27]    保留
    uint32_t PC7_SELECT        : 3;  // [28:30] PC7 功能选择 (000:Input, 001:Output, 010:NAND_RB1, 111:IO Disable)
    uint32_t RESERVED31        : 1;  // [31]    保留
} PC_CFG0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p323的 PC_CFG1_REG Register

// PC 配置寄存器 1
typedef struct {
    uint32_t PC8_SELECT        : 3;  // [0:2]   PC8 功能选择  (000:Input, 001:Output, 010:NAND_DQ0, 011:SDC2_D0, 111:IO Disable)
    uint32_t RESERVED3         : 1;  // [3]     保留
    uint32_t PC9_SELECT        : 3;  // [4:6]   PC9 功能选择  (000:Input, 001:Output, 010:NAND_DQ1, 011:SDC2_D1, 111:IO Disable)
    uint32_t RESERVED7         : 1;  // [7]     保留
    uint32_t PC10_SELECT       : 3;  // [8:10]  PC10 功能选择 (000:Input, 001:Output, 010:NAND_DQ2, 011:SDC2_D2, 111:IO Disable)
    uint32_t RESERVED11        : 1;  // [11]    保留
    uint32_t PC11_SELECT       : 3;  // [12:14] PC11 功能选择 (000:Input, 001:Output, 010:NAND_DQ3, 011:SDC2_D3, 111:IO Disable)
    uint32_t RESERVED15        : 1;  // [15]    保留
    uint32_t PC12_SELECT       : 3;  // [16:18] PC12 功能选择 (000:Input, 001:Output, 010:NAND_DQ4, 011:SDC2_D4, 111:IO Disable)
    uint32_t RESERVED19        : 1;  // [19]    保留
    uint32_t PC13_SELECT       : 3;  // [20:22] PC13 功能选择 (000:Input, 001:Output, 010:NAND_DQ5, 011:SDC2_D5, 111:IO Disable)
    uint32_t RESERVED23        : 1;  // [23]    保留
    uint32_t PC14_SELECT       : 3;  // [24:26] PC14 功能选择 (000:Input, 001:Output, 010:NAND_DQ6, 011:SDC2_D6, 111:IO Disable)
    uint32_t RESERVED27        : 1;  // [27]    保留
    uint32_t PC15_SELECT       : 3;  // [28:30] PC15 功能选择 (000:Input, 001:Output, 010:NAND_DQ7, 011:SDC2_D7, 111:IO Disable)
    uint32_t RESERVED31        : 1;  // [31]    保留
} PC_CFG1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p324的 PC_CFG2_REG Register

// PC 配置寄存器 2
typedef struct {
    uint32_t PC16_SELECT       : 3;  // [0:2]   PC16 功能选择 (000:Input, 001:Output, 010:NAND_DQS, 011:SDC2_RST, 111:IO Disable)
    uint32_t RESERVED3         : 1;  // [3]     保留
    uint32_t RESERVED4_7       : 4;  // [4:7]   保留
    uint32_t RESERVED8_10      : 3;  // [8:10]  保留（手册该位域标为“/”）
    uint32_t RESERVED11_31     : 21; // [11:31] 保留
} PC_CFG2_REG_t;


// 以下来自于 Allwinner H3 数据手册，p324的 PC_CFG3_REG Register

// PC 配置寄存器 3
typedef struct {
    uint32_t RESERVED0_31      : 32; // [0:31] 保留
} PC_CFG3_REG_t;


// 以下来自于 Allwinner H3 数据手册，p324的 PC_DATA_REG Register

// PC 数据寄存器
typedef struct {
    uint32_t PC_DAT            : 19; // [0:18]  PC 数据位
    uint32_t RESERVED19_31     : 13; // [19:31] 保留
} PC_DATA_REG_t;


// 以下来自于 Allwinner H3 数据手册，p325的 PC_DRV0_REG Register

// PC 驱动能力寄存器 0
typedef struct {
    uint32_t PC0_DRV           : 2;  // [0:1]
    uint32_t PC1_DRV           : 2;  // [2:3]
    uint32_t PC2_DRV           : 2;  // [4:5]
    uint32_t PC3_DRV           : 2;  // [6:7]
    uint32_t PC4_DRV           : 2;  // [8:9]
    uint32_t PC5_DRV           : 2;  // [10:11]
    uint32_t PC6_DRV           : 2;  // [12:13]
    uint32_t PC7_DRV           : 2;  // [14:15]
    uint32_t PC8_DRV           : 2;  // [16:17]
    uint32_t PC9_DRV           : 2;  // [18:19]
    uint32_t PC10_DRV          : 2;  // [20:21]
    uint32_t PC11_DRV          : 2;  // [22:23]
    uint32_t PC12_DRV          : 2;  // [24:25]
    uint32_t PC13_DRV          : 2;  // [26:27]
    uint32_t PC14_DRV          : 2;  // [28:29]
    uint32_t PC15_DRV          : 2;  // [30:31]
} PC_DRV0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p325的 PC_DRV1_REG Register

// PC 驱动能力寄存器 1
typedef struct {
    uint32_t PC16_DRV          : 2;  // [0:1]
    uint32_t PC17_DRV          : 2;  // [2:3]
    uint32_t PC18_DRV          : 2;  // [4:5]
    uint32_t RESERVED6_31      : 26; // [6:31] 保留
} PC_DRV1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p325的 PC_PULL0_REG Register

// PC 上下拉寄存器 0
typedef struct {
    uint32_t PC0_PULL          : 2;  // [0:1]
    uint32_t PC1_PULL          : 2;  // [2:3]
    uint32_t PC2_PULL          : 2;  // [4:5]
    uint32_t PC3_PULL          : 2;  // [6:7]
    uint32_t PC4_PULL          : 2;  // [8:9]
    uint32_t PC5_PULL          : 2;  // [10:11]
    uint32_t PC6_PULL          : 2;  // [12:13]
    uint32_t PC7_PULL          : 2;  // [14:15]
    uint32_t PC8_PULL          : 2;  // [16:17]
    uint32_t PC9_PULL          : 2;  // [18:19]
    uint32_t PC10_PULL         : 2;  // [20:21]
    uint32_t PC11_PULL         : 2;  // [22:23]
    uint32_t PC12_PULL         : 2;  // [24:25]
    uint32_t PC13_PULL         : 2;  // [26:27]
    uint32_t PC14_PULL         : 2;  // [28:29]
    uint32_t PC15_PULL         : 2;  // [30:31]
} PC_PULL0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p325的 PC_PULL1_REG Register

// PC 上下拉寄存器 1
typedef struct {
    uint32_t PC16_PULL         : 2;  // [0:1]
    uint32_t PC17_PULL         : 2;  // [2:3]
    uint32_t PC18_PULL         : 2;  // [4:5]
    uint32_t RESERVED6_31      : 26; // [6:31] 保留
} PC_PULL1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p326的 PD_CFG0_REG Register

// PD 配置寄存器 0
typedef struct {
    uint32_t PD0_SELECT        : 3;  // [0:2]   PD0 功能选择  (000:Input, 001:Output, 010:RGMII_RXD3/MII_RXD3/RMII_NULL, 011:DI_TX, 111:IO Disable)
    uint32_t RESERVED3         : 1;  // [3]     保留
    uint32_t PD1_SELECT        : 3;  // [4:6]   PD1 功能选择  (000:Input, 001:Output, 010:RGMII_RXD2/MII_RXD2/RMII_NULL, 011:DI_RX, 111:IO Disable)
    uint32_t RESERVED7         : 1;  // [7]     保留
    uint32_t PD2_SELECT        : 3;  // [8:10]  PD2 功能选择  (000:Input, 001:Output, 010:RGMII_RXD1/MII_RXD1/RMII_RXD1, 111:IO Disable)
    uint32_t RESERVED11        : 1;  // [11]    保留
    uint32_t PD3_SELECT        : 3;  // [12:14] PD3 功能选择  (000:Input, 001:Output, 010:RGMII_RXD0/MII_RXD0/RMII_RXD0, 111:IO Disable)
    uint32_t RESERVED15        : 1;  // [15]    保留
    uint32_t PD4_SELECT        : 3;  // [16:18] PD4 功能选择  (000:Input, 001:Output, 010:RGMII_RXCK/MII_RXCK/RMII_NULL, 111:IO Disable)
    uint32_t RESERVED19        : 1;  // [19]    保留
    uint32_t PD5_SELECT        : 3;  // [20:22] PD5 功能选择  (000:Input, 001:Output, 010:RGMII_RXCTL/MII_RXDV/RMII_NULL, 111:IO Disable)
    uint32_t RESERVED23        : 1;  // [23]    保留
    uint32_t PD6_SELECT        : 3;  // [24:26] PD6 功能选择  (000:Input, 001:Output, 010:RGMII_NULL/MII_RXERR/RMII_RXER, 111:IO Disable)
    uint32_t RESERVED27        : 1;  // [27]    保留
    uint32_t PD7_SELECT        : 3;  // [28:30] PD7 功能选择  (000:Input, 001:Output, 010:RGMII_TXD3/MII_TXD3/RMII_NULL, 111:IO Disable)
    uint32_t RESERVED31        : 1;  // [31]    保留
} PD_CFG0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p327的 PD_CFG1_REG Register

// PD 配置寄存器 1
typedef struct {
    uint32_t PD8_SELECT        : 3;  // [0:2]   PD8 功能选择  (000:Input, 001:Output, 010:RGMII_TXD2/MII_TXD2/RMII_NULL, 111:IO Disable)
    uint32_t RESERVED3         : 1;  // [3]     保留
    uint32_t PD9_SELECT        : 3;  // [4:6]   PD9 功能选择  (000:Input, 001:Output, 010:RGMII_TXD1/MII_TXD1/RMII_TXD1, 111:IO Disable)
    uint32_t RESERVED7         : 1;  // [7]     保留
    uint32_t PD10_SELECT       : 3;  // [8:10]  PD10 功能选择 (000:Input, 001:Output, 010:RGMII_TXD0/MII_TXD0/RMII_TXD0, 111:IO Disable)
    uint32_t RESERVED11        : 1;  // [11]    保留
    uint32_t PD11_SELECT       : 3;  // [12:14] PD11 功能选择 (000:Input, 001:Output, 010:RGMII_NULL/MII_CRS/RMII_NULL, 111:IO Disable)
    uint32_t RESERVED15        : 1;  // [15]    保留
    uint32_t PD12_SELECT       : 3;  // [16:18] PD12 功能选择 (000:Input, 001:Output, 010:RGMII_TXCK/MII_TXCK/RMII_TXCK, 111:IO Disable)
    uint32_t RESERVED19        : 1;  // [19]    保留
    uint32_t PD13_SELECT       : 3;  // [20:22] PD13 功能选择 (000:Input, 001:Output, 010:RGMII_TXCTL/MII_TXEN/RMII_TXEN, 111:IO Disable)
    uint32_t RESERVED23        : 1;  // [23]    保留
    uint32_t PD14_SELECT       : 3;  // [24:26] PD14 功能选择 (000:Input, 001:Output, 010:RGMII_NULL/MII_TXERR/RMII_NULL, 111:IO Disable)
    uint32_t RESERVED27        : 1;  // [27]    保留
    uint32_t PD15_SELECT       : 3;  // [28:30] PD15 功能选择 (000:Input, 001:Output, 010:RGMII_CLKIN/MII_COL/RMII_NULL, 111:IO Disable)
    uint32_t RESERVED31        : 1;  // [31]    保留
} PD_CFG1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p328的 PD_CFG2_REG Register

// PD 配置寄存器 2
typedef struct {
    uint32_t PD16_SELECT       : 3;  // [0:2]   PD16 功能选择 (000:Input, 001:Output, 010:MDC, 111:IO Disable)
    uint32_t RESERVED3         : 1;  // [3]     保留
    uint32_t PD17_SELECT       : 3;  // [4:6]   PD17 功能选择 (000:Input, 001:Output, 010:MDIO, 111:IO Disable)
    uint32_t RESERVED7_31      : 25; // [7:31]  保留
} PD_CFG2_REG_t;


// 以下来自于 Allwinner H3 数据手册，p328的 PD_CFG3_REG Register

// PD 配置寄存器 3
typedef struct {
    uint32_t RESERVED0_31      : 32; // [0:31] 保留
} PD_CFG3_REG_t;


// 以下来自于 Allwinner H3 数据手册，p328的 PD_DATA_REG Register

// PD 数据寄存器
typedef struct {
    uint32_t PD_DAT            : 18; // [0:17]  PD 数据位
    uint32_t RESERVED18_31     : 14; // [18:31] 保留
} PD_DATA_REG_t;


// 以下来自于 Allwinner H3 数据手册，p329的 PD_DRV0_REG Register

// PD 驱动能力寄存器 0
typedef struct {
    uint32_t PD0_DRV           : 2;  uint32_t PD1_DRV  : 2;
    uint32_t PD2_DRV           : 2;  uint32_t PD3_DRV  : 2;
    uint32_t PD4_DRV           : 2;  uint32_t PD5_DRV  : 2;
    uint32_t PD6_DRV           : 2;  uint32_t PD7_DRV  : 2;
    uint32_t PD8_DRV           : 2;  uint32_t PD9_DRV  : 2;
    uint32_t PD10_DRV          : 2;  uint32_t PD11_DRV : 2;
    uint32_t PD12_DRV          : 2;  uint32_t PD13_DRV : 2;
    uint32_t PD14_DRV          : 2;  uint32_t PD15_DRV : 2;
} PD_DRV0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p329的 PD_DRV1_REG Register

// PD 驱动能力寄存器 1
typedef struct {
    uint32_t PD16_DRV          : 2;  // [0:1]
    uint32_t PD17_DRV          : 2;  // [2:3]
    uint32_t RESERVED4_31      : 28; // [4:31] 保留
} PD_DRV1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p329的 PD_PULL0_REG Register

// PD 上下拉寄存器 0
typedef struct {
    uint32_t PD0_PULL          : 2;  uint32_t PD1_PULL  : 2;
    uint32_t PD2_PULL          : 2;  uint32_t PD3_PULL  : 2;
    uint32_t PD4_PULL          : 2;  uint32_t PD5_PULL  : 2;
    uint32_t PD6_PULL          : 2;  uint32_t PD7_PULL  : 2;
    uint32_t PD8_PULL          : 2;  uint32_t PD9_PULL  : 2;
    uint32_t PD10_PULL         : 2;  uint32_t PD11_PULL : 2;
    uint32_t PD12_PULL         : 2;  uint32_t PD13_PULL : 2;
    uint32_t PD14_PULL         : 2;  uint32_t PD15_PULL : 2;
} PD_PULL0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p329的 PD_PULL1_REG Register

// PD 上下拉寄存器 1
typedef struct {
    uint32_t PD16_PULL         : 2;  // [0:1]
    uint32_t PD17_PULL         : 2;  // [2:3]
    uint32_t RESERVED4_31      : 28; // [4:31] 保留
} PD_PULL1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p330的 PE_CFG0_REG Register

// PE 配置寄存器 0
typedef struct {
    uint32_t PE0_SELECT        : 3;  // [0:2]   PE0 功能选择 (000:Input, 001:Output, 010:CSI_PCLK, 011:TS_CLK, 111:IO Disable)
    uint32_t RESERVED3         : 1;  // [3]     保留
    uint32_t PE1_SELECT        : 3;  // [4:6]   PE1 功能选择 (000:Input, 001:Output, 010:CSI_MCLK, 011:TS_ERR, 111:IO Disable)
    uint32_t RESERVED7         : 1;  // [7]     保留
    uint32_t PE2_SELECT        : 3;  // [8:10]  PE2 功能选择 (000:Input, 001:Output, 010:CSI_HSYNC, 011:TS_SYNC, 111:IO Disable)
    uint32_t RESERVED11        : 1;  // [11]    保留
    uint32_t PE3_SELECT        : 3;  // [12:14] PE3 功能选择 (000:Input, 001:Output, 010:CSI_VSYNC, 011:TS_DVLD, 111:IO Disable)
    uint32_t RESERVED15        : 1;  // [15]    保留
    uint32_t PE4_SELECT        : 3;  // [16:18] PE4 功能选择 (000:Input, 001:Output, 010:CSI_D0, 011:TS_D0, 111:IO Disable)
    uint32_t RESERVED19        : 1;  // [19]    保留
    uint32_t PE5_SELECT        : 3;  // [20:22] PE5 功能选择 (000:Input, 001:Output, 010:CSI_D1, 011:TS_D1, 111:IO Disable)
    uint32_t RESERVED23        : 1;  // [23]    保留
    uint32_t PE6_SELECT        : 3;  // [24:26] PE6 功能选择 (000:Input, 001:Output, 010:CSI_D2, 011:TS_D2, 111:IO Disable)
    uint32_t RESERVED27        : 1;  // [27]    保留
    uint32_t PE7_SELECT        : 3;  // [28:30] PE7 功能选择 (000:Input, 001:Output, 010:CSI_D3, 011:TS_D3, 111:IO Disable)
    uint32_t RESERVED31        : 1;  // [31]    保留
} PE_CFG0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p331的 PE_CFG1_REG Register

// PE 配置寄存器 1
typedef struct {
    uint32_t PE8_SELECT        : 3;  // [0:2]   PE8 功能选择  (000:Input, 001:Output, 010:CSI_D4, 011:TS_D4, 111:IO Disable)
    uint32_t RESERVED3         : 1;  // [3]     保留
    uint32_t PE9_SELECT        : 3;  // [4:6]   PE9 功能选择  (000:Input, 001:Output, 010:CSI_D5, 011:TS_D5, 111:IO Disable)
    uint32_t RESERVED7         : 1;  // [7]     保留
    uint32_t PE10_SELECT       : 3;  // [8:10]  PE10 功能选择 (000:Input, 001:Output, 010:CSI_D6, 011:TS_D6, 111:IO Disable)
    uint32_t RESERVED11        : 1;  // [11]    保留
    uint32_t PE11_SELECT       : 3;  // [12:14] PE11 功能选择 (000:Input, 001:Output, 010:CSI_D7, 011:TS_D7, 111:IO Disable)
    uint32_t RESERVED15        : 1;  // [15]    保留
    uint32_t PE12_SELECT       : 3;  // [16:18] PE12 功能选择 (000:Input, 001:Output, 010:CSI_SCK, 011:TWI2_SCK, 111:IO Disable)
    uint32_t RESERVED19        : 1;  // [19]    保留
    uint32_t PE13_SELECT       : 3;  // [20:22] PE13 功能选择 (000:Input, 001:Output, 010:CSI_SDA, 011:TWI2_SDA, 111:IO Disable)
    uint32_t RESERVED23        : 1;  // [23]    保留
    uint32_t PE14_SELECT       : 3;  // [24:26] PE14 功能选择 (000:Input, 001:Output, 111:IO Disable)
    uint32_t RESERVED27        : 1;  // [27]    保留
    uint32_t PE15_SELECT       : 3;  // [28:30] PE15 功能选择 (000:Input, 001:Output, 111:IO Disable)
    uint32_t RESERVED31        : 1;  // [31]    保留
} PE_CFG1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p332的 PE_CFG2_REG Register

// PE 配置寄存器 2
typedef struct {
    uint32_t RESERVED0_31      : 32; // [0:31] 保留
} PE_CFG2_REG_t;


// 以下来自于 Allwinner H3 数据手册，p332的 PE_CFG3_REG Register

// PE 配置寄存器 3
typedef struct {
    uint32_t RESERVED0_31      : 32; // [0:31] 保留
} PE_CFG3_REG_t;


// 以下来自于 Allwinner H3 数据手册，p332的 PE_DATA_REG Register

// PE 数据寄存器
typedef struct {
    uint32_t PE_DAT            : 16; // [0:15]  PE 数据位
    uint32_t RESERVED16_31     : 16; // [16:31] 保留
} PE_DATA_REG_t;


// 以下来自于 Allwinner H3 数据手册，p333的 PE_DRV0_REG Register

// PE 驱动能力寄存器 0
typedef struct {
    uint32_t PE0_DRV           : 2;  uint32_t PE1_DRV  : 2;
    uint32_t PE2_DRV           : 2;  uint32_t PE3_DRV  : 2;
    uint32_t PE4_DRV           : 2;  uint32_t PE5_DRV  : 2;
    uint32_t PE6_DRV           : 2;  uint32_t PE7_DRV  : 2;
    uint32_t PE8_DRV           : 2;  uint32_t PE9_DRV  : 2;
    uint32_t PE10_DRV          : 2;  uint32_t PE11_DRV : 2;
    uint32_t PE12_DRV          : 2;  uint32_t PE13_DRV : 2;
    uint32_t PE14_DRV          : 2;  uint32_t PE15_DRV : 2;
} PE_DRV0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p333的 PE_DRV1_REG Register

// PE 驱动能力寄存器 1
typedef struct {
    uint32_t RESERVED0_31      : 32; // [0:31] 保留
} PE_DRV1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p333的 PE_PULL0_REG Register

// PE 上下拉寄存器 0
typedef struct {
    uint32_t PE0_PULL          : 2;  uint32_t PE1_PULL  : 2;
    uint32_t PE2_PULL          : 2;  uint32_t PE3_PULL  : 2;
    uint32_t PE4_PULL          : 2;  uint32_t PE5_PULL  : 2;
    uint32_t PE6_PULL          : 2;  uint32_t PE7_PULL  : 2;
    uint32_t PE8_PULL          : 2;  uint32_t PE9_PULL  : 2;
    uint32_t PE10_PULL         : 2;  uint32_t PE11_PULL : 2;
    uint32_t PE12_PULL         : 2;  uint32_t PE13_PULL : 2;
    uint32_t PE14_PULL         : 2;  uint32_t PE15_PULL : 2;
} PE_PULL0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p333的 PE_PULL1_REG Register

// PE 上下拉寄存器 1
typedef struct {
    uint32_t RESERVED0_31      : 32; // [0:31] 保留
} PE_PULL1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p334的 PF_CFG0_REG Register

// PF 配置寄存器 0
typedef struct {
    uint32_t PF0_SELECT        : 3;  // [0:2]   PF0 功能选择 (000:Input, 001:Output, 010:SDC0_D1, 011:JTAG_MS, 111:IO Disable)
    uint32_t RESERVED3         : 1;  // [3]     保留
    uint32_t PF1_SELECT        : 3;  // [4:6]   PF1 功能选择 (000:Input, 001:Output, 010:SDC0_D0, 011:JTAG_DI, 111:IO Disable)
    uint32_t RESERVED7         : 1;  // [7]     保留
    uint32_t PF2_SELECT        : 3;  // [8:10]  PF2 功能选择 (000:Input, 001:Output, 010:SDC0_CLK, 011:UART0_TX, 111:IO Disable)
    uint32_t RESERVED11        : 1;  // [11]    保留
    uint32_t PF3_SELECT        : 3;  // [12:14] PF3 功能选择 (000:Input, 001:Output, 010:SDC0_CMD, 011:JTAG_DO, 111:IO Disable)
    uint32_t RESERVED15        : 1;  // [15]    保留
    uint32_t PF4_SELECT        : 3;  // [16:18] PF4 功能选择 (000:Input, 001:Output, 010:SDC0_D3, 011:UART0_RX, 111:IO Disable)
    uint32_t RESERVED19        : 1;  // [19]    保留
    uint32_t PF5_SELECT        : 3;  // [20:22] PF5 功能选择 (000:Input, 001:Output, 010:SDC0_D2, 011:JTAG_CK, 111:IO Disable)
    uint32_t RESERVED23        : 1;  // [23]    保留
    uint32_t PF6_SELECT        : 3;  // [24:26] PF6 功能选择 (000:Input, 001:Output, 111:IO Disable)
    uint32_t RESERVED27_31     : 5;  // [27:31] 保留
} PF_CFG0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p334的 PF_CFG1_REG Register

// PF 配置寄存器 1
typedef struct {
    uint32_t RESERVED0_31      : 32; // [0:31] 保留
} PF_CFG1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p334的 PF_CFG2_REG Register

// PF 配置寄存器 2
typedef struct {
    uint32_t RESERVED0_31      : 32; // [0:31] 保留
} PF_CFG2_REG_t;


// 以下来自于 Allwinner H3 数据手册，p334的 PF_CFG3_REG Register

// PF 配置寄存器 3
typedef struct {
    uint32_t RESERVED0_31      : 32; // [0:31] 保留
} PF_CFG3_REG_t;


// 以下来自于 Allwinner H3 数据手册，p335的 PF_DATA_REG Register

// PF 数据寄存器
typedef struct {
    uint32_t PF_DAT            : 7;  // [0:6]   PF 数据位
    uint32_t RESERVED7_31      : 25; // [7:31]  保留
} PF_DATA_REG_t;


// 以下来自于 Allwinner H3 数据手册，p335的 PF_DRV0_REG Register

// PF 驱动能力寄存器 0
typedef struct {
    uint32_t PF0_DRV           : 2;  // [0:1]
    uint32_t PF1_DRV           : 2;  // [2:3]
    uint32_t PF2_DRV           : 2;  // [4:5]
    uint32_t PF3_DRV           : 2;  // [6:7]
    uint32_t PF4_DRV           : 2;  // [8:9]
    uint32_t PF5_DRV           : 2;  // [10:11]
    uint32_t PF6_DRV           : 2;  // [12:13]
    uint32_t RESERVED14_31     : 18; // [14:31] 保留
} PF_DRV0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p336的 PF_DRV1_REG Register

// PF 驱动能力寄存器 1
typedef struct {
    uint32_t RESERVED0_31      : 32; // [0:31] 保留
} PF_DRV1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p336的 PF_PULL0_REG Register

// PF 上下拉寄存器 0
typedef struct {
    uint32_t PF0_PULL          : 2;  // [0:1]
    uint32_t PF1_PULL          : 2;  // [2:3]
    uint32_t PF2_PULL          : 2;  // [4:5]
    uint32_t PF3_PULL          : 2;  // [6:7]
    uint32_t PF4_PULL          : 2;  // [8:9]
    uint32_t PF5_PULL          : 2;  // [10:11]
    uint32_t PF6_PULL          : 2;  // [12:13]
    uint32_t RESERVED14_31     : 18; // [14:31] 保留
} PF_PULL0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p336的 PF_PULL1_REG Register

// PF 上下拉寄存器 1
typedef struct {
    uint32_t RESERVED0_31      : 32; // [0:31] 保留
} PF_PULL1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p337的 PG_CFG0_REG Register

// PG 配置寄存器 0
typedef struct {
    uint32_t PG0_SELECT        : 3;  // [0:2]   PG0 功能选择 (000:Input, 001:Output, 010:SDC1_CLK, 110:PG_EINT0, 111:IO Disable)
    uint32_t RESERVED3         : 1;  // [3]     保留
    uint32_t PG1_SELECT        : 3;  // [4:6]   PG1 功能选择 (000:Input, 001:Output, 010:SDC1_CMD, 110:PG_EINT1, 111:IO Disable)
    uint32_t RESERVED7         : 1;  // [7]     保留
    uint32_t PG2_SELECT        : 3;  // [8:10]  PG2 功能选择 (000:Input, 001:Output, 010:SDC1_D0, 110:PG_EINT2, 111:IO Disable)
    uint32_t RESERVED11        : 1;  // [11]    保留
    uint32_t PG3_SELECT        : 3;  // [12:14] PG3 功能选择 (000:Input, 001:Output, 010:SDC1_D1, 110:PG_EINT3, 111:IO Disable)
    uint32_t RESERVED15        : 1;  // [15]    保留
    uint32_t PG4_SELECT        : 3;  // [16:18] PG4 功能选择 (000:Input, 001:Output, 010:SDC1_D2, 110:PG_EINT4, 111:IO Disable)
    uint32_t RESERVED19        : 1;  // [19]    保留
    uint32_t PG5_SELECT        : 3;  // [20:22] PG5 功能选择 (000:Input, 001:Output, 010:SDC1_D3, 110:PG_EINT5, 111:IO Disable)
    uint32_t RESERVED23        : 1;  // [23]    保留
    uint32_t PG6_SELECT        : 3;  // [24:26] PG6 功能选择 (000:Input, 001:Output, 010:UART1_TX, 110:PG_EINT6, 111:IO Disable)
    uint32_t RESERVED27        : 1;  // [27]    保留
    uint32_t PG7_SELECT        : 3;  // [28:30] PG7 功能选择 (000:Input, 001:Output, 010:UART1_RX, 110:PG_EINT7, 111:IO Disable)
    uint32_t RESERVED31        : 1;  // [31]    保留
} PG_CFG0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p338的 PG_CFG1_REG Register

// PG 配置寄存器 1
typedef struct {
    uint32_t PG8_SELECT        : 3;  // [0:2]   PG8 功能选择  (000:Input, 001:Output, 010:UART1_RTS, 110:PG_EINT8, 111:IO Disable)
    uint32_t RESERVED3         : 1;  // [3]     保留
    uint32_t PG9_SELECT        : 3;  // [4:6]   PG9 功能选择  (000:Input, 001:Output, 010:UART1_CTS, 110:PG_EINT9, 111:IO Disable)
    uint32_t RESERVED7         : 1;  // [7]     保留
    uint32_t PG10_SELECT       : 3;  // [8:10]  PG10 功能选择 (000:Input, 001:Output, 010:PCM1_CLK, 110:PG_EINT10, 111:IO Disable)
    uint32_t RESERVED11        : 1;  // [11]    保留
    uint32_t PG11_SELECT       : 3;  // [12:14] PG11 功能选择 (000:Input, 001:Output, 010:PCM1_CLK, 110:PG_EINT11, 111:IO Disable)
    uint32_t RESERVED15        : 1;  // [15]    保留
    uint32_t PG12_SELECT       : 3;  // [16:18] PG12 功能选择 (000:Input, 001:Output, 010:PCM1_DOUT, 110:PG_EINT12, 111:IO Disable)
    uint32_t RESERVED19        : 1;  // [19]    保留
    uint32_t PG13_SELECT       : 3;  // [20:22] PG13 功能选择 (000:Input, 001:Output, 010:PCM1_DIN, 110:PG_EINT13, 111:IO Disable)
    uint32_t RESERVED23_31     : 9;  // [23:31] 保留
} PG_CFG1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p338的 PG_CFG2_REG Register

// PG 配置寄存器 2
typedef struct {
    uint32_t RESERVED0_31      : 32; // [0:31] 保留
} PG_CFG2_REG_t;


// 以下来自于 Allwinner H3 数据手册，p338的 PG_CFG3_REG Register

// PG 配置寄存器 3
typedef struct {
    uint32_t RESERVED0_31      : 32; // [0:31] 保留
} PG_CFG3_REG_t;


// 以下来自于 Allwinner H3 数据手册，p338的 PG_DATA_REG Register

// PG 数据寄存器
typedef struct {
    uint32_t PG_DAT            : 14; // [0:13]  PG 数据位
    uint32_t RESERVED14_31     : 18; // [14:31] 保留
} PG_DATA_REG_t;


// 以下来自于 Allwinner H3 数据手册，p339的 PG_DRV0_REG Register

// PG 驱动能力寄存器 0
typedef struct {
    uint32_t PG0_DRV           : 2;  uint32_t PG1_DRV  : 2;
    uint32_t PG2_DRV           : 2;  uint32_t PG3_DRV  : 2;
    uint32_t PG4_DRV           : 2;  uint32_t PG5_DRV  : 2;
    uint32_t PG6_DRV           : 2;  uint32_t PG7_DRV  : 2;
    uint32_t PG8_DRV           : 2;  uint32_t PG9_DRV  : 2;
    uint32_t PG10_DRV          : 2;  uint32_t PG11_DRV : 2;
    uint32_t PG12_DRV          : 2;  uint32_t PG13_DRV : 2;
    uint32_t RESERVED28_31     : 4;  // [28:31] 保留
} PG_DRV0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p339的 PG_DRV1_REG Register

// PG 驱动能力寄存器 1
typedef struct {
    uint32_t RESERVED0_31      : 32; // [0:31] 保留
} PG_DRV1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p339的 PG_PULL0_REG Register

// PG 上下拉寄存器 0
typedef struct {
    uint32_t PG0_PULL          : 2;  uint32_t PG1_PULL  : 2;
    uint32_t PG2_PULL          : 2;  uint32_t PG3_PULL  : 2;
    uint32_t PG4_PULL          : 2;  uint32_t PG5_PULL  : 2;
    uint32_t PG6_PULL          : 2;  uint32_t PG7_PULL  : 2;
    uint32_t PG8_PULL          : 2;  uint32_t PG9_PULL  : 2;
    uint32_t PG10_PULL         : 2;  uint32_t PG11_PULL : 2;
    uint32_t PG12_PULL         : 2;  uint32_t PG13_PULL : 2;
    uint32_t RESERVED28_31     : 4;  // [28:31] 保留
} PG_PULL0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p339的 PG_PULL1_REG Register

// PG 上下拉寄存器 1
typedef struct {
    uint32_t RESERVED0_31      : 32; // [0:31] 保留
} PG_PULL1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p339的 PA_EINT_CFG0_REG Register

// PA 外部中断配置寄存器 0
typedef struct {
    uint32_t PA_EINT0_CFG      : 4;  // [0:3]   外部中断 0 触发方式 (0x0:上升沿, 0x1:下降沿, 0x2:高电平, 0x3:低电平, 0x4:双边沿)
    uint32_t PA_EINT1_CFG      : 4;  // [4:7]   外部中断 1 触发方式
    uint32_t PA_EINT2_CFG      : 4;  // [8:11]  外部中断 2 触发方式
    uint32_t PA_EINT3_CFG      : 4;  // [12:15] 外部中断 3 触发方式
    uint32_t PA_EINT4_CFG      : 4;  // [16:19] 外部中断 4 触发方式
    uint32_t PA_EINT5_CFG      : 4;  // [20:23] 外部中断 5 触发方式
    uint32_t PA_EINT6_CFG      : 4;  // [24:27] 外部中断 6 触发方式
    uint32_t PA_EINT7_CFG      : 4;  // [28:31] 外部中断 7 触发方式
} PA_EINT_CFG0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p340的 PA_EINT_CFG1_REG Register

// PA 外部中断配置寄存器 1
typedef struct {
    uint32_t PA_EINT8_CFG      : 4;  // [0:3]
    uint32_t PA_EINT9_CFG      : 4;  // [4:7]
    uint32_t PA_EINT10_CFG     : 4;  // [8:11]
    uint32_t PA_EINT11_CFG     : 4;  // [12:15]
    uint32_t PA_EINT12_CFG     : 4;  // [16:19]
    uint32_t PA_EINT13_CFG     : 4;  // [20:23]
    uint32_t PA_EINT14_CFG     : 4;  // [24:27]
    uint32_t PA_EINT15_CFG     : 4;  // [28:31]
} PA_EINT_CFG1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p340的 PA_EINT_CFG2_REG Register

// PA 外部中断配置寄存器 2
typedef struct {
    uint32_t PA_EINT16_CFG     : 4;  // [0:3]
    uint32_t PA_EINT17_CFG     : 4;  // [4:7]
    uint32_t PA_EINT18_CFG     : 4;  // [8:11]
    uint32_t PA_EINT19_CFG     : 4;  // [12:15]
    uint32_t PA_EINT20_CFG     : 4;  // [16:19]
    uint32_t PA_EINT21_CFG     : 4;  // [20:23]
    uint32_t RESERVED24_31     : 8;  // [24:31] 保留
} PA_EINT_CFG2_REG_t;


// 以下来自于 Allwinner H3 数据手册，p340的 PA_EINT_CFG3_REG Register

// PA 外部中断配置寄存器 3
typedef struct {
    uint32_t RESERVED0_31      : 32; // [0:31] 保留
} PA_EINT_CFG3_REG_t;


// 以下来自于 Allwinner H3 数据手册，p341的 PA_EINT_CTL_REG Register

// PA 外部中断控制寄存器
typedef struct {
    uint32_t PA_EINT_EN        : 22; // [0:21]  PA 外部中断使能 (0:Disable, 1:Enable)
    uint32_t RESERVED22_23     : 2;  // [22:23] 保留
    uint32_t RESERVED24_31     : 8;  // [24:31] 保留
} PA_EINT_CTL_REG_t;


// 以下来自于 Allwinner H3 数据手册，p341的 PA_EINT_STATUS_REG Register

// PA 外部中断状态寄存器
typedef struct {
    uint32_t PA_EINT_PENDING   : 22; // [0:21]  PA 外部中断挂起位 (0:No IRQ pending, 1:IRQ pending, 写 1 清除)
    uint32_t RESERVED22_23     : 2;  // [22:23] 保留
    uint32_t RESERVED24_31     : 8;  // [24:31] 保留
} PA_EINT_STATUS_REG_t;


// 以下来自于 Allwinner H3 数据手册，p341-342的 PA_EINT_DEB_REG Register

// PA 外部中断去抖寄存器
typedef struct {
    uint32_t PIO_INT_CLK_SELECT : 1;  // [0]    中断时钟源选择 (0:LOSC 32KHz, 1:HOSC 24MHz)
    uint32_t RESERVED1_3        : 3;  // [1:3]  保留
    uint32_t DEB_CLK_PRE_SCALE  : 3;  // [4:6]  去抖时钟预分频系数 n (实际分频为 2^n)
    uint32_t RESERVED7_31       : 25; // [7:31] 保留
} PA_EINT_DEB_REG_t;


// 以下来自于 Allwinner H3 数据手册，p342的 PG_EINT_CFG0_REG Register

// PG 外部中断配置寄存器 0
typedef struct {
    uint32_t PG_EINT0_CFG      : 4;  // [0:3]
    uint32_t PG_EINT1_CFG      : 4;  // [4:7]
    uint32_t PG_EINT2_CFG      : 4;  // [8:11]
    uint32_t PG_EINT3_CFG      : 4;  // [12:15]
    uint32_t PG_EINT4_CFG      : 4;  // [16:19]
    uint32_t PG_EINT5_CFG      : 4;  // [20:23]
    uint32_t PG_EINT6_CFG      : 4;  // [24:27]
    uint32_t PG_EINT7_CFG      : 4;  // [28:31]
} PG_EINT_CFG0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p342的 PG_EINT_CFG1_REG Register

// PG 外部中断配置寄存器 1
typedef struct {
    uint32_t PG_EINT8_CFG      : 4;  // [0:3]
    uint32_t PG_EINT9_CFG      : 4;  // [4:7]
    uint32_t PG_EINT10_CFG     : 4;  // [8:11]
    uint32_t PG_EINT11_CFG     : 4;  // [12:15]
    uint32_t PG_EINT12_CFG     : 4;  // [16:19]
    uint32_t PG_EINT13_CFG     : 4;  // [20:23]
    uint32_t RESERVED24_31     : 8;  // [24:31] 保留
} PG_EINT_CFG1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p342的 PG_EINT_CFG2_REG Register

// PG 外部中断配置寄存器 2
typedef struct {
    uint32_t RESERVED0_31      : 32; // [0:31] 保留
} PG_EINT_CFG2_REG_t;


// 以下来自于 Allwinner H3 数据手册，p342-343的 PG_EINT_CFG3_REG Register

// PG 外部中断配置寄存器 3
typedef struct {
    uint32_t RESERVED0_31      : 32; // [0:31] 保留
} PG_EINT_CFG3_REG_t;


// 以下来自于 Allwinner H3 数据手册，p343的 PG_EINT_CTL_REG Register

// PG 外部中断控制寄存器
typedef struct {
    uint32_t PG_EINT_EN        : 14; // [0:13]  PG 外部中断使能
    uint32_t RESERVED14_31     : 18; // [14:31] 保留
} PG_EINT_CTL_REG_t;


// 以下来自于 Allwinner H3 数据手册，p343的 PG_EINT_STATUS_REG Register

// PG 外部中断状态寄存器
typedef struct {
    uint32_t PG_EINT_PENDING   : 14; // [0:13]  PG 外部中断挂起位 (写 1 清除)
    uint32_t RESERVED14_31     : 18; // [14:31] 保留
} PG_EINT_STATUS_REG_t;


// 以下来自于 Allwinner H3 数据手册，p344的 PG_EINT_DEB_REG Register

// PG 外部中断去抖寄存器
typedef struct {
    uint32_t PIO_INT_CLK_SELECT : 1;  // [0]    中断时钟源选择 (0:LOSC 32KHz, 1:HOSC 24MHz)
    uint32_t RESERVED1_3        : 3;  // [1:3]  保留
    uint32_t DEB_CLK_PRE_SCALE  : 3;  // [4:6]  去抖时钟预分频系数 n (实际分频为 2^n)
    uint32_t RESERVED7_31       : 25; // [7:31] 保留
} PG_EINT_DEB_REG_t;

// 这部分内容来自 Allwinner_H3_Datasheet_v1.2.pdf，page 345-350，CPUs-PORT(PL) Register List / Register Description

// Allwinner H3 PL（CPUs-PORT）模块基地址
#define R_PIO_BASE_ADDR                 0x01F02C00  // CPUs-PORT(PIO) 基地址

// ========================= PL 寄存器偏移 =========================
#define PL_CFG0_REG_OFFSET              0x0000  // Port L 配置寄存器 0
#define PL_CFG1_REG_OFFSET              0x0004  // Port L 配置寄存器 1
#define PL_CFG2_REG_OFFSET              0x0008  // Port L 配置寄存器 2
#define PL_CFG3_REG_OFFSET              0x000C  // Port L 配置寄存器 3
#define PL_DAT_REG_OFFSET               0x0010  // Port L 数据寄存器
#define PL_DRV0_REG_OFFSET              0x0014  // Port L 多驱动能力寄存器 0
#define PL_DRV1_REG_OFFSET              0x0018  // Port L 多驱动能力寄存器 1
#define PL_PUL0_REG_OFFSET              0x001C  // Port L 上下拉寄存器 0
#define PL_PUL1_REG_OFFSET              0x0020  // Port L 上下拉寄存器 1

// ========================= PL 外部中断寄存器偏移 =========================
#define PL_EINT_CFG0_REG_OFFSET         0x0200  // PL 外部中断配置寄存器 0
#define PL_EINT_CFG1_REG_OFFSET         0x0204  // PL 外部中断配置寄存器 1
#define PL_EINT_CFG2_REG_OFFSET         0x0208  // PL 外部中断配置寄存器 2
#define PL_EINT_CFG3_REG_OFFSET         0x020C  // PL 外部中断配置寄存器 3
#define PL_EINT_CTL_REG_OFFSET          0x0210  // PL 外部中断控制寄存器
#define PL_EINT_STA_REG_OFFSET          0x0214  // PL 外部中断状态寄存器
#define PL_EINT_DEB_REG_OFFSET          0x0218  // PL 外部中断去抖寄存器


// 以下来自于 Allwinner H3 数据手册，p346的 PL_CFG0_REG Register

// PL 配置寄存器 0
typedef struct {
    uint32_t PL0_SELECT         : 3;  // [0:2]   PL0 功能选择 (000:Input, 001:Output, 010:S_TWI_SCK, 011:Reserved, 100:Reserved, 101:Reserved, 110:S_PL_EINT0, 111:IO Disable)
    uint32_t RESERVED3          : 1;  // [3]     保留
    uint32_t PL1_SELECT         : 3;  // [4:6]   PL1 功能选择 (000:Input, 001:Output, 010:S_TWI_SDA, 011:Reserved, 100:Reserved, 101:Reserved, 110:S_PL_EINT1, 111:IO Disable)
    uint32_t RESERVED7          : 1;  // [7]     保留
    uint32_t PL2_SELECT         : 3;  // [8:10]  PL2 功能选择 (000:Input, 001:Output, 010:S_UART_TX, 011:Reserved, 100:Reserved, 101:Reserved, 110:S_PL_EINT2, 111:IO Disable)
    uint32_t RESERVED11         : 1;  // [11]    保留
    uint32_t PL3_SELECT         : 3;  // [12:14] PL3 功能选择 (000:Input, 001:Output, 010:S_UART_RX, 011:Reserved, 100:Reserved, 101:Reserved, 110:S_PL_EINT3, 111:IO Disable)
    uint32_t RESERVED15         : 1;  // [15]    保留
    uint32_t PL4_SELECT         : 3;  // [16:18] PL4 功能选择 (000:Input, 001:Output, 010:S_JTAG_MS, 011:Reserved, 100:Reserved, 101:Reserved, 110:S_PL_EINT4, 111:IO Disable)
    uint32_t RESERVED19         : 1;  // [19]    保留
    uint32_t PL5_SELECT         : 3;  // [20:22] PL5 功能选择 (000:Input, 001:Output, 010:S_JTAG_CK, 011:Reserved, 100:Reserved, 101:Reserved, 110:S_PL_EINT5, 111:IO Disable)
    uint32_t RESERVED23         : 1;  // [23]    保留
    uint32_t PL6_SELECT         : 3;  // [24:26] PL6 功能选择 (000:Input, 001:Output, 010:S_JTAG_DO, 011:Reserved, 100:Reserved, 101:Reserved, 110:S_PL_EINT6, 111:IO Disable)
    uint32_t RESERVED27         : 1;  // [27]    保留
    uint32_t PL7_SELECT         : 3;  // [28:30] PL7 功能选择 (000:Input, 001:Output, 010:S_JTAG_DI, 011:Reserved, 100:Reserved, 101:Reserved, 110:S_PL_EINT7, 111:IO Disable)
    uint32_t RESERVED31         : 1;  // [31]    保留
} PL_CFG0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p347的 PL_CFG1_REG Register

// PL 配置寄存器 1
typedef struct {
    uint32_t PL8_SELECT         : 3;  // [0:2]   PL8 功能选择 (000:Input, 001:Output, 010:Reserved, 011:Reserved, 100:Reserved, 101:Reserved, 110:S_PL_EINT8, 111:IO Disable)
    uint32_t RESERVED3          : 1;  // [3]     保留
    uint32_t PL9_SELECT         : 3;  // [4:6]   PL9 功能选择 (000:Input, 001:Output, 010:Reserved, 011:Reserved, 100:Reserved, 101:Reserved, 110:S_PL_EINT9, 111:IO Disable)
    uint32_t RESERVED7          : 1;  // [7]     保留
    uint32_t PL10_SELECT        : 3;  // [8:10]  PL10 功能选择 (000:Input, 001:Output, 010:S_PWM, 011:Reserved, 100:Reserved, 101:Reserved, 110:S_PL_EINT10, 111:IO Disable)
    uint32_t RESERVED11         : 1;  // [11]    保留
    uint32_t PL11_SELECT        : 3;  // [12:14] PL11 功能选择 (000:Input, 001:Output, 010:S_CIR_RX, 011:Reserved, 100:Reserved, 101:Reserved, 110:S_PL_EINT11, 111:IO Disable)
    uint32_t RESERVED15_31      : 17; // [15:31] 保留
} PL_CFG1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p347的 PL_CFG2_REG Register

// PL 配置寄存器 2
typedef struct {
    uint32_t RESERVED0_31       : 32; // [0:31] 保留
} PL_CFG2_REG_t;


// 以下来自于 Allwinner H3 数据手册，p348的 PL_CFG3_REG Register

// PL 配置寄存器 3
typedef struct {
    uint32_t RESERVED0_31       : 32; // [0:31] 保留
} PL_CFG3_REG_t;


// 以下来自于 Allwinner H3 数据手册，p348的 PL_DATA_REG Register

// PL 数据寄存器
typedef struct {
    uint32_t PL_DAT             : 12; // [0:11]  PL 数据位 (输入模式下为引脚状态，输出模式下为软件设置值，复用功能模式下读取值未定义)
    uint32_t RESERVED12_31      : 20; // [12:31] 保留
} PL_DATA_REG_t;


// 以下来自于 Allwinner H3 数据手册，p348的 PL_DRV0_REG Register

// PL 多驱动能力寄存器 0
typedef struct {
    uint32_t PL0_DRV            : 2;  // [0:1]   PL0 驱动等级选择 (00:Level 0, 01:Level 1, 10:Level 2, 11:Level 3)
    uint32_t PL1_DRV            : 2;  // [2:3]   PL1 驱动等级选择
    uint32_t PL2_DRV            : 2;  // [4:5]   PL2 驱动等级选择
    uint32_t PL3_DRV            : 2;  // [6:7]   PL3 驱动等级选择
    uint32_t PL4_DRV            : 2;  // [8:9]   PL4 驱动等级选择
    uint32_t PL5_DRV            : 2;  // [10:11] PL5 驱动等级选择
    uint32_t PL6_DRV            : 2;  // [12:13] PL6 驱动等级选择
    uint32_t PL7_DRV            : 2;  // [14:15] PL7 驱动等级选择
    uint32_t PL8_DRV            : 2;  // [16:17] PL8 驱动等级选择
    uint32_t PL9_DRV            : 2;  // [18:19] PL9 驱动等级选择
    uint32_t PL10_DRV           : 2;  // [20:21] PL10 驱动等级选择
    uint32_t PL11_DRV           : 2;  // [22:23] PL11 驱动等级选择
    uint32_t RESERVED24_31      : 8;  // [24:31] 保留
} PL_DRV0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p348的 PL_DRV1_REG Register

// PL 多驱动能力寄存器 1
typedef struct {
    uint32_t RESERVED0_31       : 32; // [0:31] 保留
} PL_DRV1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p348的 PL_PULL0_REG Register

// PL 上下拉寄存器 0
typedef struct {
    uint32_t PL0_PULL           : 2;  // [0:1]   PL0 上下拉选择 (00:Pull-up/down disable, 01:Pull-up, 10:Pull-down, 11:Reserved)
    uint32_t PL1_PULL           : 2;  // [2:3]   PL1 上下拉选择
    uint32_t PL2_PULL           : 2;  // [4:5]   PL2 上下拉选择
    uint32_t PL3_PULL           : 2;  // [6:7]   PL3 上下拉选择
    uint32_t PL4_PULL           : 2;  // [8:9]   PL4 上下拉选择
    uint32_t PL5_PULL           : 2;  // [10:11] PL5 上下拉选择
    uint32_t PL6_PULL           : 2;  // [12:13] PL6 上下拉选择
    uint32_t PL7_PULL           : 2;  // [14:15] PL7 上下拉选择
    uint32_t PL8_PULL           : 2;  // [16:17] PL8 上下拉选择
    uint32_t PL9_PULL           : 2;  // [18:19] PL9 上下拉选择
    uint32_t PL10_PULL          : 2;  // [20:21] PL10 上下拉选择
    uint32_t PL11_PULL          : 2;  // [22:23] PL11 上下拉选择
    uint32_t RESERVED24_31      : 8;  // [24:31] 保留
} PL_PULL0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p349的 PL_PULL1_REG Register

// PL 上下拉寄存器 1
typedef struct {
    uint32_t RESERVED0_31       : 32; // [0:31] 保留
} PL_PULL1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p349的 PL_EINT_CFG0_REG Register

// PL 外部中断配置寄存器 0
typedef struct {
    uint32_t PL_EINT0_CFG       : 4;  // [0:3]   外部中断 0 触发方式 (0x0: Positive Edge, 0x1: Negative Edge, 0x2: High Level, 0x3: Low Level, 0x4: Double Edge)
    uint32_t PL_EINT1_CFG       : 4;  // [4:7]   外部中断 1 触发方式
    uint32_t PL_EINT2_CFG       : 4;  // [8:11]  外部中断 2 触发方式
    uint32_t PL_EINT3_CFG       : 4;  // [12:15] 外部中断 3 触发方式
    uint32_t PL_EINT4_CFG       : 4;  // [16:19] 外部中断 4 触发方式
    uint32_t PL_EINT5_CFG       : 4;  // [20:23] 外部中断 5 触发方式
    uint32_t PL_EINT6_CFG       : 4;  // [24:27] 外部中断 6 触发方式
    uint32_t PL_EINT7_CFG       : 4;  // [28:31] 外部中断 7 触发方式
} PL_EINT_CFG0_REG_t;


// 以下来自于 Allwinner H3 数据手册，p349的 PL_EINT_CFG1_REG Register

// PL 外部中断配置寄存器 1
typedef struct {
    uint32_t PL_EINT8_CFG       : 4;  // [0:3]   外部中断 8 触发方式
    uint32_t PL_EINT9_CFG       : 4;  // [4:7]   外部中断 9 触发方式
    uint32_t PL_EINT10_CFG      : 4;  // [8:11]  外部中断 10 触发方式
    uint32_t PL_EINT11_CFG      : 4;  // [12:15] 外部中断 11 触发方式
    uint32_t RESERVED16_19      : 4;  // [16:19] 保留
    uint32_t RESERVED20_31      : 12; // [20:31] 保留
} PL_EINT_CFG1_REG_t;


// 以下来自于 Allwinner H3 数据手册，p350的 PL_EINT_CFG2_REG Register

// PL 外部中断配置寄存器 2
typedef struct {
    uint32_t RESERVED0_31       : 32; // [0:31] 保留
} PL_EINT_CFG2_REG_t;


// 以下来自于 Allwinner H3 数据手册，p350的 PL_EINT_CFG3_REG Register

// PL 外部中断配置寄存器 3
typedef struct {
    uint32_t RESERVED0_31       : 32; // [0:31] 保留
} PL_EINT_CFG3_REG_t;


// 以下来自于 Allwinner H3 数据手册，p350的 PL_EINT_CTL_REG Register

// PL 外部中断控制寄存器
typedef struct {
    uint32_t PL_EINT_EN         : 12; // [0:11]  PL 外部中断使能 (0: Disable, 1: Enable)
    uint32_t RESERVED12_31      : 20; // [12:31] 保留
} PL_EINT_CTL_REG_t;


// 以下来自于 Allwinner H3 数据手册，p350的 PL_EINT_STA_REG Register

// PL 外部中断状态寄存器
typedef struct {
    uint32_t PL_EINT_PENDING    : 12; // [0:11]  PL 外部中断挂起位 (0: No IRQ pending, 1: IRQ pending, 写 1 清除)
    uint32_t RESERVED12_31      : 20; // [12:31] 保留
} PL_EINT_STATUS_REG_t;


// 以下来自于 Allwinner H3 数据手册，p350的 PL_EINT_DEB_REG Register

// PL 外部中断去抖寄存器
typedef struct {
    uint32_t PIO_INT_CLK_SELECT : 1;  // [0]    中断时钟源选择 (0: LOSC 32KHz, 1: HOSC 24MHz)
    uint32_t RESERVED1_3        : 3;  // [1:3]  保留
    uint32_t DEB_CLK_PRE_SCALE  : 3;  // [4:6]  去抖时钟预分频系数 n (实际分频为 2^n)
    uint32_t RESERVED7_31       : 25; // [7:31] 保留
} PL_EINT_DEB_REG_t;

#endif //HOS_GPIO_DATASHEET_H
