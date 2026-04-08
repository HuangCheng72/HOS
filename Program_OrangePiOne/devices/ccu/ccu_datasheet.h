//
// Created by huangcheng on 2025/3/11.
//

#ifndef HOS_CCU_DATASHEET_H
#define HOS_CCU_DATASHEET_H

#include "../../lib/type.h"

// 这部分内容来自 Allwinner_H3_Datasheet_v1.2.pdf，page 90-91，CCU Register List

// Allwinner H3 CCU（Clock Control Unit）模块基地址

#define CCU_BASE_ADDR               0x01C20000  // CCU（时钟控制单元）基地址

// ========================= PLL（Phase-Locked Loop）控制寄存器 =========================
// 这些寄存器用于配置不同模块的 PLL（锁相环），PLL 负责提供稳定的时钟源。

#define PLL_CPUX_CTRL_REG_OFFSET      0x0000  // CPUX PLL 控制寄存器
#define PLL_AUDIO_CTRL_REG_OFFSET     0x0008  // AUDIO PLL 控制寄存器
#define PLL_VIDEO_CTRL_REG_OFFSET     0x0010  // VIDEO PLL 控制寄存器
#define PLL_VE_CTRL_REG_OFFSET        0x0018  // VE PLL 控制寄存器
#define PLL_DDR_CTRL_REG_OFFSET       0x0020  // DDR PLL 控制寄存器
#define PLL_PERIPH0_CTRL_REG_OFFSET   0x0028  // PERIPH0 PLL 控制寄存器
#define PLL_GPU_CTRL_REG_OFFSET       0x0038  // GPU PLL 控制寄存器
#define PLL_PERIPH1_CTRL_REG_OFFSET   0x0044  // PERIPH1 PLL 控制寄存器
#define PLL_DE_CTRL_REG_OFFSET        0x0048  // DE PLL 控制寄存器

// ========================= CPU & AXI 总线配置寄存器 =========================
// 这些寄存器用于配置 CPU、AXI、AHB、APB 总线的时钟分频和来源

#define CPUX_AXI_CFG_REG_OFFSET       0x0050  // CPUX/AXI 配置寄存器
#define AHBP1_APB1_CFG_REG_OFFSET     0x0054  // AHB1/APB1 配置寄存器
#define APB2_CFG_REG_OFFSET           0x0058  // APB2 配置寄存器
#define AHB2_CFG_REG_OFFSET           0x005C  // AHB2 配置寄存器

// ========================= 总线时钟门控寄存器 =========================
// 这些寄存器用于控制各个外设模块的时钟使能

#define BUS_CLK_GATING_REG0_OFFSET    0x0060  // 总线时钟门控寄存器 0
#define BUS_CLK_GATING_REG1_OFFSET    0x0064  // 总线时钟门控寄存器 1
#define BUS_CLK_GATING_REG2_OFFSET    0x0068  // 总线时钟门控寄存器 2
#define BUS_CLK_GATING_REG3_OFFSET    0x006C  // 总线时钟门控寄存器 3
#define BUS_CLK_GATING_REG4_OFFSET    0x0070  // 总线时钟门控寄存器 4

// ========================= 传感器与存储设备时钟寄存器 =========================
// 这些寄存器用于控制温湿度传感器、NAND 存储设备的时钟配置

#define THS_CLK_REG_OFFSET            0x0074  // 温湿度传感器时钟寄存器
#define NAND_CLK_REG_OFFSET           0x0080  // NAND 时钟寄存器

// ========================= SD/MMC 存储时钟寄存器 =========================
// 用于 SD 卡、MMC 设备的时钟控制

#define SDMMC0_CLK_REG_OFFSET         0x0088  // SDMMC0 时钟寄存器
#define SDMMC1_CLK_REG_OFFSET         0x008C  // SDMMC1 时钟寄存器
#define SDMMC2_CLK_REG_OFFSET         0x0090  // SDMMC2 时钟寄存器

// ========================= 加密与 SPI 相关时钟寄存器 =========================
// 这些寄存器控制加密模块、SPI 通信总线的时钟

#define CE_CLK_REG_OFFSET             0x009C  // CE（加密引擎）时钟寄存器
#define SPI0_CLK_REG_OFFSET           0x00A0  // SPI0 时钟寄存器
#define SPI1_CLK_REG_OFFSET           0x00A4  // SPI1 时钟寄存器

// ========================= 音频时钟寄存器 =========================
// 控制 I2S、OWA 相关的时钟

#define I2S_PCM0_CLK_REG_OFFSET       0x00B0  // I2S/PCM0 时钟寄存器
#define I2S_PCM1_CLK_REG_OFFSET       0x00B4  // I2S/PCM1 时钟寄存器
#define I2S_PCM2_CLK_REG_OFFSET       0x00B8  // I2S/PCM2 时钟寄存器
#define OWA_CLK_REG_OFFSET            0x00C0  // OWA（单线音频）时钟寄存器

// ========================= USBPHY 相关时钟寄存器 =========================

#define USBPHY_CFG_REG_OFFSET         0x00CC  // USBPHY 配置寄存器

// ========================= DRAM 相关时钟寄存器 =========================

#define DRAM_CFG_REG_OFFSET           0x00F4  // DRAM 配置寄存器
#define MBUS_RST_REG_OFFSET           0x00FC  // MBUS 复位寄存器
#define DRAM_CLK_GATING_REG_OFFSET    0x0100  // DRAM 时钟门控寄存器

// ========================= 多媒体相关时钟寄存器 =========================

#define TCON0_CLK_REG_OFFSET          0x0118  // TCON0（时序控制器）时钟寄存器
#define TVE_CLK_REG_OFFSET            0x0120  // TVE（电视输出）时钟寄存器
#define DEINTERLACE_CLK_REG_OFFSET    0x0124  // 去隔行时钟寄存器
#define CSI_MISC_CLK_REG_OFFSET       0x0130  // CSI_MISC（摄像头）时钟寄存器
#define CSI_CLK_REG_OFFSET            0x0134  // CSI（摄像头）时钟寄存器
#define VE_CLK_REG_OFFSET             0x013C  // VE（视频引擎）时钟寄存器
#define AC_DIG_CLK_REG_OFFSET         0x0140  // AC Digital 时钟寄存器
#define AVS_CLK_REG_OFFSET            0x0144  // AVS 时钟寄存器
#define HDMI_CLK_REG_OFFSET           0x0150  // HDMI 时钟寄存器
#define HDMI_SLOW_CLK_REG_OFFSET      0x0154  // HDMI Slow 时钟寄存器
#define MBUS_CLK_REG_OFFSET           0x015C  // MBUS 时钟寄存器
#define GPU_CLK_REG_OFFSET            0x01A0  // GPU 时钟寄存器

// ========================= PLL 偏置寄存器 =========================

#define PLL_STABLE_TIME0_REG_OFFSET   0x0200  // PLL 稳定时间寄存器 0
#define PLL_STABLE_TIME1_REG_OFFSET   0x0204  // PLL 稳定时间寄存器 1
#define PLL_CPUX_BIAS_REG_OFFSET      0x0220  // PLL CPUX 偏置寄存器
#define PLL_AUDIO_BIAS_REG_OFFSET     0x0224  // PLL AUDIO 偏置寄存器
#define PLL_VIDEO_BIAS_REG_OFFSET     0x0228  // PLL VIDEO 偏置寄存器
#define PLL_VE_BIAS_REG_OFFSET        0x022C  // PLL VE 偏置寄存器
#define PLL_DDR_BIAS_REG_OFFSET       0x0230  // PLL DDR 偏置寄存器
#define PLL_PERIPH0_BIAS_REG_OFFSET   0x0234  // PLL PERIPH0 偏置寄存器
#define PLL_GPU_BIAS_REG_OFFSET       0x023C  // PLL GPU 偏置寄存器
#define PLL_PERIPH1_BIAS_REG_OFFSET   0x0244  // PLL PERIPH1 偏置寄存器
#define PLL_DE_BIAS_REG_OFFSET        0x0248  // PLL DE 偏置寄存器

// ========================= PLL 调谐寄存器 =========================

#define PLL_CPUX_TUN_REG_OFFSET       0x0250  // PLL CPUX 调谐寄存器
#define PLL_DDR_TUN_REG_OFFSET        0x0260  // PLL DDR 调谐寄存器

// ========================= PLL 模式控制寄存器 =========================

#define PLL_CPUX_PAT_CTRL_REG_OFFSET  0x0280  // PLL CPUX 模式控制寄存器
#define PLL_AUDIO_PAT_CTRL_REG_OFFSET 0x0284  // PLL AUDIO 模式控制寄存器
#define PLL_VIDEO_PAT_CTRL_REG_OFFSET 0x0288  // PLL VIDEO 模式控制寄存器
#define PLL_VE_PAT_CTRL_REG_OFFSET    0x028C  // PLL VE 模式控制寄存器
#define PLL_DDR_PAT_CTRL_REG0_OFFSET  0x0290  // PLL DDR 模式控制寄存器 0
#define PLL_GPU_PAT_CTRL_REG_OFFSET   0x029C  // PLL GPU 模式控制寄存器
#define PLL_PERIPH1_PAT_CTRL_REG1_OFFSET  0x02A4  // PLL PERIPH1 模式控制寄存器
#define PLL_DE_PAT_CTRL_REG_OFFSET    0x02A8  // PLL DE 模式控制寄存器

// ========================= 总线软复位与安全控制寄存器 =========================
#define BUS_SOFT_RST_REG0_OFFSET      0x02C0  // 总线软复位寄存器 0
#define BUS_SOFT_RST_REG1_OFFSET      0x02C4  // 总线软复位寄存器 1
#define BUS_SOFT_RST_REG2_OFFSET      0x02C8  // 总线软复位寄存器 2
#define BUS_SOFT_RST_REG3_OFFSET      0x02D0  // 总线软复位寄存器 3
#define BUS_SOFT_RST_REG4_OFFSET      0x02D8  // 总线软复位寄存器 4
#define CCU_SEC_SWITCH_REG_OFFSET     0x02F0  // CCU 安全切换寄存器

// ========================= PS（功率控制）寄存器 =========================
#define PS_CTRL_REG_OFFSET            0x0300  // PS 控制寄存器
#define PS_CNT_REG_OFFSET             0x0304  // PS 计数寄存器


// 以下来自于 Allwinner H3 数据手册，p92的 PLL_CPUX_CTRL_REG Register

// PLL 输出频率 = 24MHz * N * K / (M * P)，默认 408MHz
typedef struct {
    uint32_t PLL_FACTOR_M      : 2;  // [0:1] PLL 乘法因子 M (M=Factor+1), 取值 1~4
    uint32_t RESERVED2_3       : 2;  // [2:3] 保留
    uint32_t PLL_FACTOR_K      : 2;  // [4:5] PLL 乘法因子 K (K=Factor+1), 取值 1~4
    uint32_t RESERVED6_7       : 2;  // [6:7] 保留
    uint32_t PLL_FACTOR_N      : 5;  // [8:12] PLL 乘法因子 N (Factor=0,N=1; Factor=31,N=32)
    uint32_t RESERVED13_15     : 3;  // [13:15] 保留
    uint32_t PLL_OUT_EXT_DVP   : 2;  // [16:17] PLL 输出外部分频 P (000:/1, 001:/2, 010:/4，仅在输出小于 288MHz 时生效)
    uint32_t RESERVED18_23     : 6;  // [18:23] 保留
    uint32_t CPUX_SDM_EN       : 1;  // [24] CPUX SDM 使能 (0=禁用, 1=启用)
    uint32_t RESERVED25_27     : 3;  // [25:27] 保留
    uint32_t LOCK              : 1;  // [28] PLL 锁定状态 (0=未锁定, 1=已锁定，表示 PLL 运行稳定)
    uint32_t RESERVED29_30     : 2;  // [29:30] 保留
    uint32_t PLL_ENABLE        : 1;  // [31] PLL 使能 (0=禁用, 1=启用)
} PLL_CPUX_CTRL_REG_t;

// 以下来自于 Allwinner H3 数据手册，p93的 PLL_AUDIO_CTRL_REG Register

// PLL_AUDIO 输出频率计算公式：
// PLL_AUDIO = (24MHz * N) / (M * P)
// PLL_AUDIO(8X) = (24MHz * N * 2) / M
// PLL_AUDIO(4X) = PLL_AUDIO(8X) / 2
// PLL_AUDIO(2X) = PLL_AUDIO(4X) / 2
// PLL 输出范围：20MHz ~ 200MHz，默认值 24.571MHz
typedef struct {
    uint32_t PLL_PREDIV_M     : 5;  // [0:4] PLL 预分频因子 M (M=Factor+1), 取值 1~32
    uint32_t RESERVED5_7      : 3;  // [5:7] 保留
    uint32_t PLL_FACTOR_N     : 7;  // [8:14] PLL 乘法因子 N (Factor=0,N=1; Factor=127,N=128)
    uint32_t RESERVED15       : 1;  // [15] 保留
    uint32_t PLL_POSTDIV_P    : 4;  // [16:19] PLL 后分频因子 P (P=Factor+1), 取值 1~16
    uint32_t RESERVED20_23    : 4;  // [20:23] 保留
    uint32_t PLL_SDM_EN       : 1;  // [24] PLL SDM 使能 (0=禁用, 1=启用, 启用时 PLL_FACTOR_N 仅低 4 位有效, 取值 1~16)
    uint32_t RESERVED25_27    : 3;  // [25:27] 保留
    uint32_t LOCK             : 1;  // [28] PLL 锁定状态 (0=未锁定, 1=已锁定，表示 PLL 运行稳定)
    uint32_t RESERVED29_30    : 2;  // [29:30] 保留
    uint32_t PLL_ENABLE       : 1;  // [31] PLL 使能 (0=禁用, 1=启用)
} PLL_AUDIO_CTRL_REG_t;

// 以下来自于 Allwinner H3 数据手册，p94的 PLL_VIDEO_CTRL_REG Register

// PLL_VIDEO 输出频率计算公式：
// 在整数模式下：PLL_OUTPUT = (24MHz * N) / M
// 在小数模式下，PLL 输出会按 25 选择
// PLL(1X) 输出 = PLL
// PLL(2X) 输出 = PLL * 2
// PLL 输出范围：30MHz ~ 600MHz，默认值 297MHz
typedef struct {
    uint32_t PLL_PREDIV_M   : 4;  // [0:3] PLL 预分频因子 M (M=Factor+1), 取值 1~16
    uint32_t RESERVED4_7    : 4;  // [4:7] 保留
    uint32_t PLL_FACTOR_N   : 7;  // [8:14] PLL 乘法因子 N (Factor=0,N=1; Factor=127,N=128)
    uint32_t RESERVED15_19  : 5;  // [15:19] 保留
    uint32_t PLL_SDM_EN     : 1;  // [20] PLL SDM 使能 (0=禁用, 1=启用)
    uint32_t RESERVED21_23  : 3;  // [21:23] 保留
    uint32_t PLL_MODE_SEL   : 1;  // [24] PLL 模式选择 (0=小数模式, 1=整数模式)
    uint32_t FRAC_CLK_OUT   : 1;  // [25] FRAC_CLK 输出选择 (0=270MHz, 1=297MHz)
    uint32_t RESERVED26_27  : 2;  // [26:27] 保留
    uint32_t LOCK           : 1;  // [28] PLL 锁定状态 (0=未锁定, 1=已锁定，表示 PLL 运行稳定)
    uint32_t RESERVED29     : 1;  // [29] 保留
    uint32_t PLL_MODE       : 1;  // [30] PLL 工作模式 (0=手动模式, 1=自动模式, 由 DE 控制)
    uint32_t PLL_ENABLE     : 1;  // [31] PLL 使能 (0=禁用, 1=启用)
} PLL_VIDEO_CTRL_REG_t;

// 以下来自于 Allwinner H3 数据手册，p95的 PLL_VE_CTRL_REG Register

// PLL_VE 输出频率计算公式：
// 在整数模式下：PLL_OUTPUT = (24MHz * N) / M
// 在小数模式下，PLL 输出会按 25 选择
// PLL 输出范围：30MHz ~ 600MHz，默认值 297MHz
typedef struct {
    uint32_t PLL_PREDIV_M   : 4;  // [0:3] PLL 预分频因子 M (M=Factor+1), 取值 1~16
    uint32_t RESERVED4_7    : 4;  // [4:7] 保留
    uint32_t PLL_FACTOR_N   : 7;  // [8:14] PLL 乘法因子 N (Factor=0,N=1; Factor=127,N=128)
    uint32_t RESERVED15_19  : 5;  // [15:19] 保留
    uint32_t PLL_SDM_EN     : 1;  // [20] PLL SDM 使能 (0=禁用, 1=启用)
    uint32_t RESERVED21_23  : 3;  // [21:23] 保留
    uint32_t PLL_MODE_SEL   : 1;  // [24] PLL 模式选择 (0=小数模式, 1=整数模式)
    uint32_t FRAC_CLK_OUT   : 1;  // [25] FRAC_CLK 输出选择 (0=270MHz, 1=297MHz)
    uint32_t RESERVED26_27  : 2;  // [26:27] 保留
    uint32_t LOCK           : 1;  // [28] PLL 锁定状态 (0=未锁定, 1=已锁定，表示 PLL 运行稳定)
    uint32_t RESERVED29_30  : 2;  // [29:30] 保留
    uint32_t PLL_ENABLE     : 1;  // [31] PLL 使能 (0=禁用, 1=启用)
} PLL_VE_CTRL_REG_t;

// 以下来自于 Allwinner H3 数据手册，p96的 PLL_DDR_CTRL_REG Register

// PLL_DDR 输出频率计算公式：
// PLL_OUTPUT = (24MHz * N * K) / M
// PLL 输出范围：200MHz ~ 2.6GHz，默认值 408MHz
typedef struct {
    uint32_t PLL_FACTOR_M         : 2;  // [0:1] PLL 乘法因子 M (M=Factor+1), 取值 1~4
    uint32_t RESERVED2_3          : 2;  // [2:3] 保留
    uint32_t PLL_FACTOR_K         : 2;  // [4:5] PLL 乘法因子 K (K=Factor+1), 取值 1~4
    uint32_t RESERVED6_7          : 2;  // [6:7] 保留
    uint32_t PLL_FACTOR_N         : 5;  // [8:12] PLL 乘法因子 N (Factor=0,N=1; Factor=31,N=32)
    uint32_t RESERVED13_19        : 7;  // [13:19] 保留
    uint32_t PLL_DDR_CFG_UPDATE   : 1;  // [20] PLL_DDR 配置更新 (0=无效, 1=更新配置)
    uint32_t RESERVED21_23        : 3;  // [21:23] 保留
    uint32_t PLL_SDM_EN           : 1;  // [24] PLL SDM 使能 (0=禁用, 1=启用)
    uint32_t RESERVED25_27        : 3;  // [25:27] 保留
    uint32_t LOCK                 : 1;  // [28] PLL 锁定状态 (0=未锁定, 1=已锁定，表示 PLL 运行稳定)
    uint32_t RESERVED29_30        : 2;  // [29:30] 保留
    uint32_t PLL_ENABLE           : 1;  // [31] PLL 使能 (0=禁用, 1=启用)
} PLL_DDR_CTRL_REG_t;

// 以下来自于 Allwinner H3 数据手册，p97的 PLL_PERIPH0_CTRL_REG Register

// PLL_PERIPH0 输出频率计算公式：
// PLL_OUTPUT = 24MHz * N * K / 2
// 当 PLL_BYPASS_EN 使能时，PLL 输出为 24MHz
// 在 Clock Control Module 中：PLL(2X) 输出 = PLL * 2 = 24MHz * N * K
// PLL 输出范围：200MHz ~ 1.8GHz，默认值 600MHz
typedef struct {
    uint32_t PLL_FACTOR_M       : 2;  // [0:1] PLL 乘法因子 M (M=Factor+1), 取值 1~4，仅在 plltset debug 有效
    uint32_t RESERVED2_3        : 2;  // [2:3] 保留
    uint32_t PLL_FACTOR_K       : 2;  // [4:5] PLL 乘法因子 K (K=Factor+1), 取值 1~4
    uint32_t RESERVED6_7        : 2;  // [6:7] 保留
    uint32_t PLL_FACTOR_N       : 5;  // [8:12] PLL 乘法因子 N (Factor=0,N=1; Factor=31,N=32)
    uint32_t RESERVED13_15      : 3;  // [13:15] 保留
    uint32_t PLL_24M_POST_DIV   : 2;  // [16:17] PLL 24M 输出时钟后分频 (当使用 25MHz 晶振时，分频因子 1/2/3/4)
    uint32_t PLL_24M_OUT_EN     : 1;  // [18] PLL 24MHz 输出使能 (0=禁用, 1=启用，25MHz 晶振时可输出 24MHz)
    uint32_t RESERVED20_23      : 5;  // [19:23] 保留
    uint32_t PLL_CLK_OUT_EN     : 1;  // [24] PLL 时钟输出使能 (0=禁用, 1=启用)
    uint32_t PLL_BYPASS_EN      : 1;  // [25] PLL 输出旁路使能 (0=禁用, 1=启用，旁路时 PLL 输出 24MHz)
    uint32_t RESERVED26_27      : 2;  // [26:27] 保留
    uint32_t LOCK               : 1;  // [28] PLL 锁定状态 (0=未锁定, 1=已锁定，表示 PLL 运行稳定)
    uint32_t RESERVED29_30      : 2;  // [29:30] 保留
    uint32_t PLL_ENABLE         : 1;  // [31] PLL 使能 (0=禁用, 1=启用)
} PLL_PERIPH0_CTRL_REG_t;

// 以下来自于 Allwinner H3 数据手册，p98的 PLL_GPU_CTRL_REG Register

// PLL_GPU 输出频率计算公式：
// 在整数模式下：PLL_GPU_OUTPUT = (24MHz * N) / M
// 在小数模式下，PLL_GPU 输出会按 25 选择
// PLL 输出范围：30MHz ~ 600MHz，默认值 297MHz
typedef struct {
    uint32_t PLL_PRE_DIV_M   : 4;  // [0:3] PLL 预分频因子 M (M=Factor+1), 取值 1~16
    uint32_t RESERVED4_7     : 4;  // [4:7] 保留
    uint32_t PLL_FACTOR_N    : 7;  // [8:14] PLL 乘法因子 N (Factor=0,N=1; Factor=127,N=128)
    uint32_t RESERVED15_19   : 5;  // [15:19] 保留
    uint32_t PLL_SDM_EN      : 1;  // [20] PLL SDM 使能 (0=禁用, 1=启用)
    uint32_t RESERVED21_23   : 3;  // [21:23] 保留
    uint32_t PLL_MODE_SEL    : 1;  // [24] PLL 模式选择 (0=小数模式, 1=整数模式)
    uint32_t FRAC_CLK_OUT    : 1;  // [25] FRAC_CLK 输出选择 (0=270MHz, 1=297MHz)
    uint32_t RESERVED26_27   : 2;  // [26:27] 保留
    uint32_t LOCK            : 1;  // [28] PLL 锁定状态 (0=未锁定, 1=已锁定，表示 PLL 运行稳定)
    uint32_t RESERVED29_30   : 2;  // [29:30] 保留
    uint32_t PLL_ENABLE      : 1;  // [31] PLL 使能 (0=禁用, 1=启用)
} PLL_GPU_CTRL_REG_t;

// 以下来自于 Allwinner H3 数据手册，p99的 PLL_PERIPH1_CTRL_REG Register

// PLL_PERIPH1 输出频率计算公式：
// PLL_OUTPUT = 24MHz * N * K / 2
// 当 PLL_BYPASS_EN 使能时，PLL 输出为 24MHz
// 在 Clock Control Module 中：PLL(2X) 输出 = PLL * 2 = 24MHz * N * K
// PLL 输出范围：200MHz ~ 1.8GHz，默认值 600MHz
typedef struct {
    uint32_t PLL_FACTOR_M       : 2;  // [0:1] PLL 乘法因子 M (M=Factor+1), 取值 1~4，仅在 plltset debug 有效
    uint32_t RESERVED2_3        : 2;  // [2:3] 保留
    uint32_t PLL_FACTOR_K       : 2;  // [4:5] PLL 乘法因子 K (K=Factor+1), 取值 1~4
    uint32_t RESERVED6_7        : 2;  // [6:7] 保留
    uint32_t PLL_FACTOR_N       : 5;  // [8:12] PLL 乘法因子 N (Factor=0,N=1; Factor=31,N=32)
    uint32_t RESERVED13_15      : 3;  // [13:15] 保留
    uint32_t PLL_24M_POST_DIV   : 2;  // [16:17] PLL 24M 输出时钟后分频 (当使用 25MHz 晶振时，分频因子 1/2/3/4)
    uint32_t PLL_24M_OUT_EN     : 1;  // [18] PLL 24MHz 输出使能 (0=禁用, 1=启用，25MHz 晶振时可输出 24MHz)
    uint32_t RESERVED19         : 1;  // [19] 保留
    uint32_t PLL_SDM_EN         : 1;  // [20] PLL SDM 使能 (0=禁用, 1=启用，PLL Σ-Δ 调制使能，开启可以降低时钟抖动)
    uint32_t RESERVED21_23      : 3;  // [21:23] 保留
    uint32_t PLL_CLK_OUT_EN     : 1;  // [24] PLL 时钟输出使能 (0=禁用, 1=启用)
    uint32_t PLL_BYPASS_EN      : 1;  // [25] PLL 输出旁路使能 (0=禁用, 1=启用，旁路时 PLL 输出 24MHz)
    uint32_t RESERVED26_27      : 2;  // [26:27] 保留
    uint32_t LOCK               : 1;  // [28] PLL 锁定状态 (0=未锁定, 1=已锁定，表示 PLL 运行稳定)
    uint32_t RESERVED29_30      : 2;  // [29:30] 保留
    uint32_t PLL_ENABLE         : 1;  // [31] PLL 使能 (0=禁用, 1=启用)
} PLL_PERIPH1_CTRL_REG_t;

// 以下来自于 Allwinner H3 数据手册，p100的 PLL_DE_CTRL_REG Register

// PLL_DE 输出频率计算公式：
// 在整数模式下：PLL_OUTPUT = (24MHz * N) / M
// 在小数模式下，PLL 输出会按 25 选择
// PLL 输出范围：30MHz ~ 600MHz，默认值 297MHz
typedef struct {
    uint32_t PLL_PRE_DIV_M   : 4;  // [0:3] PLL 预分频因子 M (M=Factor+1), 取值 1~16
    uint32_t RESERVED4_7     : 4;  // [4:7] 保留
    uint32_t PLL_FACTOR_N    : 7;  // [8:14] PLL 乘法因子 N (Factor=0,N=1; Factor=127,N=128)
    uint32_t RESERVED15_19   : 5;  // [15:19] 保留
    uint32_t PLL_SDM_EN      : 1;  // [20] PLL SDM 使能 (0=禁用, 1=启用，PLL Σ-Δ 调制使能，开启可以降低时钟抖动)
    uint32_t RESERVED21_23   : 3;  // [21:23] 保留
    uint32_t PLL_MODE_SEL    : 1;  // [24] PLL 模式选择 (0=小数模式, 1=整数模式)
    uint32_t FRAC_CLK_OUT    : 1;  // [25] FRAC_CLK 输出选择 (0=270MHz, 1=297MHz)
    uint32_t RESERVED26_27   : 2;  // [26:27] 保留
    uint32_t LOCK            : 1;  // [28] PLL 锁定状态 (0=未锁定, 1=已锁定，表示 PLL 运行稳定)
    uint32_t RESERVED29_30   : 2;  // [29:30] 保留
    uint32_t PLL_ENABLE      : 1;  // [31] PLL 使能 (0=禁用, 1=启用)
} PLL_DE_CTRL_REG_t;

// 以下来自于 Allwinner H3 数据手册，p101的 CPUX_AXI_CFG_REG Register

// CPUX/AXI 配置寄存器
typedef struct {
    uint32_t AXI_CLK_DIV_RATIO : 2;  // [0:1] AXI 时钟分频比 (00:/1, 01:/2, 10:/3, 11:/4)
    uint32_t RESERVED2_7       : 6;  // [2:7] 保留
    uint32_t CPU_APB_CLK_DIV   : 2;  // [8:9] CPU APB 时钟分频比 (00:/1, 01:/2, 1x:/4)
    uint32_t RESERVED10_15     : 6;  // [10:15] 保留
    uint32_t CPUX_CLK_SRC_SEL  : 2;  // [16:17] CPUX 时钟源选择 (00: LOSC, 01: OSC24M, 1X: PLL_CPUX)
    uint32_t RESERVED18_31     : 14; // [18:31] 保留
} CPUX_AXI_CFG_REG_t;

// 以下来自于 Allwinner H3 数据手册，p102的 AHB1_APB1_CFG_REG Register

// AHB1/APB1 配置寄存器
typedef struct {
    uint32_t RESERVED0_3        : 4;  // [0:3]  保留
    uint32_t AHB1_CLK_DIV_RATIO : 2;  // [4:5]  AHB1 时钟分频比 (00:/1, 01:/2, 10:/4, 11:/8)
    uint32_t AHB1_PRE_DIV       : 2;  // [6:7]  AHB1 预分频比 (00:/1, 01:/2, 10:/3, 11:/4)
    uint32_t APB1_CLK_RATIO     : 2;  // [8:9]  APB1 时钟分频比 (00:/2, 01:/2, 10:/4, 11:/8)
    uint32_t RESERVED10_11      : 2;  // [10:11] 保留
    uint32_t AHB1_CLK_SRC_SEL   : 2;  // [12:13] AHB1 时钟源选择 (00: LOSC, 01: OSC24M, 10: AXI, 11: PLL_PERIPH0/AHB1_PRE_DIV)
    uint32_t RESERVED14_31      : 18; // [14:31] 保留
} AHB1_APB1_CFG_REG_t;

// 以下来自于 Allwinner H3 数据手册，p102的 APB2_CFG_REG Register

// APB2 配置寄存器
typedef struct {
    uint32_t CLK_RAT_M        : 5;  // [0:4]  时钟分频比 M (M+1), 取值范围 1~32
    uint32_t RESERVED5_15     : 11; // [5:15] 保留
    uint32_t CLK_RAT_N        : 2;  // [16:17] 预分频比 N (时钟源被 2^N 预分频，取值 1/2/4/8)
    uint32_t RESERVED18_23    : 6;  // [18:23] 保留
    uint32_t APB2_CLK_SRC_SEL : 2;  // [24:25] APB2 时钟源选择 (00: LOSC, 01: OSC24M, 1X: PLL_PERIPH0)
    uint32_t RESERVED26_31    : 6;  // [26:31] 保留
} APB2_CFG_REG_t;

// 以下来自于 Allwinner H3 数据手册，p103的 AHB2_CFG_REG Register

// AHB2 配置寄存器
typedef struct {
    uint32_t AHB2_CLK_CFG   : 2;  // [0:1] AHB2 时钟源选择 (00: AHB1 Clock, 01: PLL_PERIPH0 / 2, 1X: /)
    uint32_t RESERVED2_31   : 30; // [2:31] 保留
} AHB2_CFG_REG_t;

// 以下来自于 Allwinner H3 数据手册，p103的 BUS_CLK_GATING_REG0 Register

// 总线时钟门控寄存器 0
typedef struct {
    uint32_t RESERVED0_4            : 5;  // [0:4]  保留
    uint32_t CE_GATING              : 1;  // [5]    CE 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t DMA_GATING             : 1;  // [6]    DMA 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t RESERVED7              : 1;  // [7]    保留
    uint32_t MMC0_GATING            : 1;  // [8]    MMC0 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t MMC1_GATING            : 1;  // [9]    MMC1 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t MMC2_GATING            : 1;  // [10]   MMC2 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t RESERVED11_12          : 2;  // [11:12] 保留
    uint32_t NAND_GATING            : 1;  // [13]   NAND 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t DRAM_GATING            : 1;  // [14]   DRAM 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t RESERVED15_16          : 2;  // [15:16] 保留
    uint32_t EMAC_GATING            : 1;  // [17]   EMAC 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t TS_GATING              : 1;  // [18]   TS 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t HSTMR_GATING           : 1;  // [19]   高速定时器时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t SPI0_GATING            : 1;  // [20]   SPI0 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t SPI1_GATING            : 1;  // [21]   SPI1 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t RESERVED22             : 1;  // [22]   保留
    uint32_t USB_OTG_DEVICE_GATING  : 1;  // [23]   USB OTG 设备时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t USB_OTG_EHCI0_GATING   : 1;  // [24]   USB OTG EHCI0 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t USB_EHCI1_GATING       : 1;  // [25]   USB EHCI1 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t USB_EHCI2_GATING       : 1;  // [26]   USB EHCI2 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t USB_EHCI3_GATING       : 1;  // [27]   USB EHCI3 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t USB_OTG_OHCI0_GATING   : 1;  // [28]   USB OTG OHCI0 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t USB_OHCI1_GATING       : 1;  // [29]   USB OHCI1 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t USB_OHCI2_GATING       : 1;  // [30]   USB OHCI2 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t USB_OHCI3_GATING       : 1;  // [31]   USB OHCI3 时钟门控 (0: 屏蔽, 1: 通过)
} BUS_CLK_GATING_REG0_t;

// 以下来自于 Allwinner H3 数据手册，p105的 BUS_CLK_GATING_REG1 Register

// 总线时钟门控寄存器 1
typedef struct {
    uint32_t VE_GATING          : 1;  // [0]  VE 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t RESERVED1_2        : 2;  // [1:2] 保留
    uint32_t TCON0_GATING       : 1;  // [3]  TCON0 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t TCON1_GATING       : 1;  // [4]  TCON1 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t DEINTERLACE_GATING : 1;  // [5]  去隔行时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t RESERVED6_7        : 2;  // [6:7] 保留
    uint32_t CSI_GATING         : 1;  // [8]  CSI 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t TVE_GATING         : 1;  // [9]  TVE 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t RESERVED10         : 1;  // [10] 保留
    uint32_t HDMI_GATING        : 1;  // [11] HDMI 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t DE_GATING          : 1;  // [12] DE (显示引擎) 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t RESERVED13_19      : 7;  // [13:19] 保留
    uint32_t GPU_GATING         : 1;  // [20] GPU 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t MSGBOX_GATING      : 1;  // [21] 消息框 (Message Box) 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t SPINLOCK_GATING    : 1;  // [22] 自旋锁 (Spinlock) 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t RESERVED23_31      : 9;  // [23:31] 保留
} BUS_CLK_GATING_REG1_t;

// 以下来自于 Allwinner H3 数据手册，p106的 BUS_CLK_GATING_REG2 Register

// 总线时钟门控寄存器 2
typedef struct {
    uint32_t AC_DIG_GATING     : 1;  // [0]  AC 数字音频时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t OWA_GATING        : 1;  // [1]  OWA 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t RESERVED2_4       : 3;  // [2:4] 保留
    uint32_t PIO_GATING        : 1;  // [5]  PIO (可编程 IO) 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t RESERVED6_7       : 2;  // [6:7] 保留
    uint32_t THS_GATING        : 1;  // [8]  THS (温度传感器) 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t RESERVED9_11      : 3;  // [9:11] 保留
    uint32_t I2S_PCM0_GATING   : 1;  // [12] I2S/PCM0 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t I2S_PCM1_GATING   : 1;  // [13] I2S/PCM1 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t I2S_PCM2_GATING   : 1;  // [14] I2S/PCM2 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t RESERVED15_31     : 17; // [15:31] 保留
} BUS_CLK_GATING_REG2_t;

// 以下来自于 Allwinner H3 数据手册，p107的 BUS_CLK_GATING_REG3 Register

// 总线时钟门控寄存器 3
typedef struct {
    uint32_t TWI0_GATING     : 1;  // [0]  TWI0 (I2C 总线 0) 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t TWI1_GATING     : 1;  // [1]  TWI1 (I2C 总线 1) 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t TWI2_GATING     : 1;  // [2]  TWI2 (I2C 总线 2) 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t RESERVED3_15    : 13; // [3:15] 保留
    uint32_t UART0_GATING    : 1;  // [16] UART0 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t UART1_GATING    : 1;  // [17] UART1 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t UART2_GATING    : 1;  // [18] UART2 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t UART3_GATING    : 1;  // [19] UART3 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t SCR_GATING      : 1;  // [20] SCR (智能卡接口) 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t RESERVED21_31   : 11; // [21:31] 保留
} BUS_CLK_GATING_REG3_t;

// 以下来自于 Allwinner H3 数据手册，p108的 BUS_CLK_GATING_REG4 Register

// 总线时钟门控寄存器 4
typedef struct {
    uint32_t EPHY_GATING     : 1;  // [0]  EPHY (以太网 PHY) 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t RESERVED1_6     : 6;  // [1:6] 保留
    uint32_t DBGSYS_GATING   : 1;  // [7]  DBGSYS (调试系统) 时钟门控 (0: 屏蔽, 1: 通过)
    uint32_t RESERVED8_31    : 24; // [8:31] 保留
} BUS_CLK_GATING_REG4_t;

// 以下来自于 Allwinner H3 数据手册，p109的 THS_CLK_REG Register

// THS (温度传感器) 时钟寄存器
typedef struct {
    uint32_t THS_CLK_DIV_RATIO : 2;  // [0:1] THS 时钟分频比 (00:/1, 01:/2, 10:/4, 11:/6)
    uint32_t RESERVED2_23      : 22; // [2:23] 保留
    uint32_t THS_CLK_SRC_SEL   : 2;  // [24:25] THS 时钟源选择 (00: OSC24M, 01: /, 10: /, 11: /)
    uint32_t RESERVED26_30     : 5;  // [26:30] 保留
    uint32_t SCLK_GATING       : 1;  // [31]  特殊时钟门控 (0: 关闭, 1: 开启)
} THS_CLK_REG_t;

// 以下来自于 Allwinner H3 数据手册，p109的 NAND_CLK_REG Register

// NAND 时钟寄存器
typedef struct {
    uint32_t CLK_DIV_RATIO_M  : 4;  // [0:3]  NAND 时钟分频因子 M (分频比 = M+1), 取值范围 1~16
    uint32_t RESERVED4_15     : 12; // [4:15] 保留
    uint32_t CLK_DIV_RATIO_N  : 2;  // [16:17] NAND 预分频因子 N (00:/1, 01:/2, 10:/4, 11:/8)
    uint32_t RESERVED18_23    : 6;  // [18:23] 保留
    uint32_t CLK_SRC_SEL      : 2;  // [24:25] NAND 时钟源选择 (00: OSC24M, 01: PLL_PERIPH0, 10: PLL_PERIPH1, 11: 未定义)
    uint32_t RESERVED26_30    : 5;  // [26:30] 保留
    uint32_t SCLK_GATING      : 1;  // [31]  NAND 特殊时钟门控 (0: 关闭, 1: 开启)
} NAND_CLK_REG_t;

// 以下来自于 Allwinner H3 数据手册，p110的 SDMMC0_CLK_REG Register

// SDMMC0 时钟寄存器
typedef struct {
    uint32_t CLK_DIV_RATIO_M        : 4;  // [0:3] 时钟分频因子 M (分频比 = M+1), 取值范围 1~16
    uint32_t RESERVED4_7            : 4;  // [4:7] 保留
    uint32_t OUTPUT_CLK_PHASE_CTR   : 3;  // [8:10] 输出时钟相位控制 (基于源时钟延迟，从0到7)
    uint32_t RESERVED11_15          : 5;  // [11:15] 保留
    uint32_t CLK_DIV_RATIO_N        : 2;  // [16:17] 预分频因子 N (00:/1, 01:/2, 10:/4, 11:/8)
    uint32_t RESERVED18_19          : 2;  // [18:19] 保留
    uint32_t SAMPLE_CLK_PHASE_CTR   : 3;  // [20:22] 采样时钟相位控制 (基于源时钟延迟，从0到7)
    uint32_t RESERVED23             : 1;  // [23] 保留
    uint32_t CLK_SRC_SEL            : 2;  // [24:25] 时钟源选择 (00: OSC24M, 01: PLL_PERIPH0, 10: PLL_PERIPH1)
    uint32_t RESERVED26_30          : 5;  // [26:30] 保留
    uint32_t SCLK_GATING            : 1;  // [31] 特殊时钟门控 (0: 关闭, 1: 开启)
} SDMMC0_CLK_REG_t;

// 以下来自于 Allwinner H3 数据手册，p111的 SDMMC1_CLK_REG Register

// SDMMC1 时钟寄存器
typedef struct {
    uint32_t CLK_DIV_RATIO_M        : 4;  // [0:3] 时钟分频因子 M (分频比 = M+1), 取值范围 1~16
    uint32_t RESERVED4_7            : 4;  // [4:7] 保留
    uint32_t OUTPUT_CLK_PHASE_CTR   : 3;  // [8:10] 输出时钟相位控制 (基于源时钟延迟，从0到7)
    uint32_t RESERVED11_15          : 5;  // [11:15] 保留
    uint32_t CLK_DIV_RATIO_N        : 2;  // [16:17] 预分频因子 N (00:/1, 01:/2, 10:/4, 11:/8)
    uint32_t RESERVED18_19          : 2;  // [18:19] 保留
    uint32_t SAMPLE_CLK_PHASE_CTR   : 3;  // [20:22] 采样时钟相位控制 (基于源时钟延迟，从0到7)
    uint32_t RESERVED23             : 1;  // [23] 保留
    uint32_t CLK_SRC_SEL            : 2;  // [24:25] 时钟源选择 (00: OSC24M, 01: PLL_PERIPH0, 10: PLL_PERIPH1)
    uint32_t RESERVED26_29          : 4;  // [26:29] 保留
    uint32_t MMC1_MODE_SELECT       : 1;  // [30] 模式选择 (0: 旧模式, 1: 新模式)
    uint32_t SCLK_GATING            : 1;  // [31] 特殊时钟门控 (0: 关闭, 1: 开启)
} SDMMC1_CLK_REG_t;

// 以下来自于 Allwinner H3 数据手册，p112的 SDMMC2_CLK_REG Register

// SDMMC2 时钟寄存器
typedef struct {
    uint32_t CLK_DIV_RATIO_M        : 4;  // [0:3] 时钟分频因子 M (分频比 = M+1), 取值范围 1~16
    uint32_t RESERVED4_7            : 4;  // [4:7] 保留
    uint32_t OUTPUT_CLK_PHASE_CTR   : 3;  // [8:10] 输出时钟相位控制 (基于源时钟延迟，从0到7)
    uint32_t RESERVED11_15          : 5;  // [11:15] 保留
    uint32_t CLK_DIV_RATIO_N        : 2;  // [16:17] 预分频因子 N (00:/1, 01:/2, 10:/4, 11:/8)
    uint32_t RESERVED18_19          : 2;  // [18:19] 保留
    uint32_t SAMPLE_CLK_PHASE_CTR   : 3;  // [20:22] 采样时钟相位控制 (基于源时钟延迟，从0到7)
    uint32_t RESERVED23             : 1;  // [23] 保留
    uint32_t CLK_SRC_SEL            : 2;  // [24:25] 时钟源选择 (00: OSC24M, 01: PLL_PERIPH0, 10: PLL_PERIPH1)
    uint32_t RESERVED26_29          : 4;  // [26:29] 保留
    uint32_t MMC2_MODE_SELECT       : 1;  // [30] 模式选择 (0: 旧模式, 1: 新模式)
    uint32_t SCLK_GATING            : 1;  // [31] 特殊时钟门控 (0: 关闭, 1: 开启)
} SDMMC2_CLK_REG_t;

// 以下来自于 Allwinner H3 数据手册，p113的 TS_CLK_REG Register

// TS 时钟寄存器
typedef struct {
    uint32_t CLK_DIV_RATIO_M : 4;  // [0:3] 时钟分频因子 M (分频比 = M+1), 取值范围 1~16
    uint32_t RESERVED4_15    : 12; // [4:15] 保留
    uint32_t CLK_DIV_RATIO_N : 2;  // [16:17] 预分频因子 N (时钟源先除以 2^N, 即 00:/1, 01:/2, 10:/4, 11:/8)
    uint32_t RESERVED18_23   : 6;  // [18:23] 保留
    uint32_t CLK_SRC_SEL     : 4;  // [24:27] 时钟源选择 (0000: OSC24M, 0001: PLL_PERIPH0, 其他: /)
    uint32_t RESERVED28_30   : 3;  // [28:30] 保留
    uint32_t SCLK_GATING     : 1;  // [31] 特殊时钟门控 (最大时钟 200MHz, 0: 关闭, 1: 开启)
} TS_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p113的 CE_CLK_REG Register

// CE (加密引擎) 时钟寄存器
typedef struct {
    uint32_t CLK_DIV_RATIO_M : 4;  // [0:3] 时钟分频因子 M (分频比 = M+1), 取值范围 1~16
    uint32_t RESERVED4_15    : 12; // [4:15] 保留
    uint32_t CLK_DIV_RATIO_N : 2;  // [16:17] 预分频因子 N (时钟源先除以 2^N, 即 00:/1, 01:/2, 10:/4, 11:/8)
    uint32_t RESERVED18_23   : 6;  // [18:23] 保留
    uint32_t CLK_SRC_SEL     : 2;  // [24:25] 时钟源选择 (00: OSC24M, 01: PLL_PERIPH0, 10: PLL_PERIPH1, 11: /)
    uint32_t RESERVED26_30   : 5;  // [26:30] 保留
    uint32_t SCLK_GATING     : 1;  // [31] 特殊时钟门控 (最大时钟 400MHz, 0: 关闭, 1: 开启)
} CE_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p114的 SPI0_CLK_REG Register

// SPI0 时钟寄存器
typedef struct {
    uint32_t CLK_DIV_RATIO_M : 4;  // [0:3] 时钟分频因子 M (分频比 = M+1), 取值范围 1~16
    uint32_t RESERVED4_15    : 12; // [4:15] 保留
    uint32_t CLK_DIV_RATIO_N : 2;  // [16:17] 预分频因子 N (时钟源先除以 2^N, 即 00:/1, 01:/2, 10:/4, 11:/8)
    uint32_t RESERVED18_23   : 6;  // [18:23] 保留
    uint32_t CLK_SRC_SEL     : 2;  // [24:25] 时钟源选择 (00: OSC24M, 01: PLL_PERIPH0, 10: PLL_PERIPH1, 11: /)
    uint32_t RESERVED26_30   : 5;  // [26:30] 保留
    uint32_t SCLK_GATING     : 1;  // [31] 特殊时钟门控 (最大时钟 200MHz, 0: 关闭, 1: 开启)
} SPI0_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p114-115的 SPI1_CLK_REG Register

// SPI1 时钟寄存器
typedef struct {
    uint32_t CLK_DIV_RATIO_M : 4;  // [0:3] 时钟分频因子 M (分频比 = M+1), 取值范围 1~16
    uint32_t RESERVED4_15    : 12; // [4:15] 保留
    uint32_t CLK_DIV_RATIO_N : 2;  // [16:17] 预分频因子 N (时钟源先除以 2^N, 即 00:/1, 01:/2, 10:/4, 11:/8)
    uint32_t RESERVED18_23   : 6;  // [18:23] 保留
    uint32_t CLK_SRC_SEL     : 2;  // [24:25] 时钟源选择 (00: OSC24M, 01: PLL_PERIPH0, 10: PLL_PERIPH1, 11: /)
    uint32_t RESERVED26_30   : 5;  // [26:30] 保留
    uint32_t SCLK_GATING     : 1;  // [31] 特殊时钟门控 (最大时钟 200MHz, 0: 关闭, 1: 开启)
} SPI1_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p115的 I2S/PCM 0_CLK_REG Register

// I2S/PCM0 时钟寄存器
typedef struct {
    uint32_t RESERVED0_15  : 16; // [0:15] 保留
    uint32_t CLK_SRC_SEL   : 2;  // [16:17] 时钟源选择 (00: PLL_AUDIO(8X), 01: PLL_AUDIO(8X)/2, 10: PLL_AUDIO(8X)/4, 11: PLL_AUDIO)
    uint32_t RESERVED18_30 : 13; // [18:30] 保留
    uint32_t SCLK_GATING   : 1;  // [31] 特殊时钟门控 (最大时钟 200MHz, 0: 关闭, 1: 开启)
} I2S_PCM0_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p116的 I2S/PCM 1_CLK_REG Register

// I2S/PCM1 时钟寄存器
typedef struct {
    uint32_t RESERVED0_15  : 16; // [0:15] 保留
    uint32_t CLK_SRC_SEL   : 2;  // [16:17] 时钟源选择 (00: PLL_AUDIO(8X), 01: PLL_AUDIO(8X)/2, 10: PLL_AUDIO(8X)/4, 11: PLL_AUDIO)
    uint32_t RESERVED18_30 : 13; // [18:30] 保留
    uint32_t SCLK_GATING   : 1;  // [31] 特殊时钟门控 (最大时钟 200MHz, 0: 关闭, 1: 开启)
} I2S_PCM1_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p116的 I2S/PCM 2_CLK_REG Register

// I2S/PCM2 时钟寄存器
typedef struct {
    uint32_t RESERVED0_15  : 16; // [0:15] 保留
    uint32_t CLK_SRC_SEL   : 2;  // [16:17] 时钟源选择 (00: PLL_AUDIO(8X), 01: PLL_AUDIO(8X)/2, 10: PLL_AUDIO(8X)/4, 11: PLL_AUDIO)
    uint32_t RESERVED18_30 : 13; // [18:30] 保留
    uint32_t SCLK_GATING   : 1;  // [31] 特殊时钟门控 (最大时钟 200MHz, 0: 关闭, 1: 开启)
} I2S_PCM2_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p116-117的 OWA_CLK_REG Register

// OWA 时钟寄存器
typedef struct {
    uint32_t CLK_DIV_RATIO_M : 4;  // [0:3] 时钟分频因子 M (分频比 = M+1), 取值范围 1~16
    uint32_t RESERVED4_30    : 27; // [4:30] 保留
    uint32_t SCLK_GATING     : 1;  // [31] 特殊时钟门控 (最大时钟 200MHz, 0: 关闭, 1: 开启)
} OWA_CLK_REG_t;

// 以下来自于 Allwinner H3 数据手册，p117的 USBPHY_CFG_REG Register

// USBPHY 配置寄存器
typedef struct {
    uint32_t USBPHY0_RST             : 1;  // [0] USB PHY0 复位控制 (0: Assert, 1: De-assert)
    uint32_t USBPHY1_RST             : 1;  // [1] USB PHY1 复位控制 (0: Assert, 1: De-assert)
    uint32_t USBPHY2_RST             : 1;  // [2] USB PHY2 复位控制 (0: Assert, 1: De-assert)
    uint32_t USBPHY3_RST             : 1;  // [3] USB PHY3 复位控制 (0: Assert, 1: De-assert)
    uint32_t RESERVED4_7             : 4;  // [4:7] 保留
    uint32_t SCLK_GATING_USBPHY0     : 1;  // [8] USB PHY0 特殊时钟门控 (0: Clock is OFF, 1: Clock is ON)
    uint32_t SCLK_GATING_USBPHY1     : 1;  // [9] USB PHY1 特殊时钟门控 (0: Clock is OFF, 1: Clock is ON)
    uint32_t SCLK_GATING_USBPHY2     : 1;  // [10] USB PHY2 特殊时钟门控 (0: Clock is OFF, 1: Clock is ON)
    uint32_t SCLK_GATING_USBPHY3     : 1;  // [11] USB PHY3 特殊时钟门控 (0: Clock is OFF, 1: Clock is ON)
    uint32_t RESERVED12_15           : 4;  // [12:15] 保留
    uint32_t SCLK_GATING_OTG_OHCI0   : 1;  // [16] USB OTG_OHCI0 特殊时钟门控 (0: Clock is OFF, 1: Clock is ON)
    uint32_t SCLK_GATING_OHCI1       : 1;  // [17] OHCI1 特殊时钟门控 (0: Clock is OFF, 1: Clock is ON)
    uint32_t SCLK_GATING_OHCI2       : 1;  // [18] OHCI2 特殊时钟门控 (0: Clock is OFF, 1: Clock is ON)
    uint32_t SCLK_GATING_OHCI3       : 1;  // [19] OHCI3 特殊时钟门控 (0: Clock is OFF, 1: Clock is ON)
    uint32_t RESERVED20_31           : 12; // [20:31] 保留
} USBPHY_CFG_REG_t;


// 以下来自于 Allwinner H3 数据手册，p118的 DRAM_CFG_REG Register

// DRAM 配置寄存器
typedef struct {
    uint32_t DRAM_DIV_M      : 4;  // [0:3] DRAM 时钟分频因子 M (分频比 = M+1), 取值范围 1~16
    uint32_t RESERVED4_15    : 12; // [4:15] 保留
    uint32_t SDRCLK_UPD      : 1;  // [16] SDRCLK 配置更新 (0: Invalid, 1: Valid，置 1 后配置生效，生效后自动清零)
    uint32_t RESERVED17_19   : 3;  // [17:19] 保留
    uint32_t CLK_SRC_SEL     : 2;  // [20:21] 时钟源选择 (00: PLL_DDR, 01: PLL_PERIPH0(2X), 其他: /)
    uint32_t RESERVED22_30   : 9;  // [22:30] 保留
    uint32_t DRAM_CTR_RST    : 1;  // [31] DRAM 控制器 AHB 时钟域复位 (0: Assert, 1: De-assert)
} DRAM_CFG_REG_t;


// 以下来自于 Allwinner H3 数据手册，p119的 MBUS_RST_REG Register

// MBUS 复位寄存器
typedef struct {
    uint32_t RESERVED0_30    : 31; // [0:30] 保留
    uint32_t MBUS_RESET      : 1;  // [31] MBUS 域复位控制 (0: Reset Mbus Domain, 1: Assert Mbus Domain)
} MBUS_RST_REG_t;


// 以下来自于 Allwinner H3 数据手册，p119的 DRAM_CLK_GATING_REG Register

// DRAM 时钟门控寄存器
typedef struct {
    uint32_t VE_DCLK_GATING              : 1;  // [0] VE 的 DRAM 时钟门控 (0: Mask, 1: Pass)
    uint32_t CSI_DCLK_GATING             : 1;  // [1] CSI 的 DRAM 时钟门控 (0: Mask, 1: Pass)
    uint32_t DEINTERLACE_DCLK_GATING     : 1;  // [2] DEINTERLACE 的 DRAM SCLK(1X) 门控 (0: Mask, 1: Pass)
    uint32_t TS_DCLK_GATING              : 1;  // [3] TS 的 DRAM 时钟门控 (0: Mask, 1: Pass)
    uint32_t RESERVED4_31                : 28; // [4:31] 保留
} DRAM_CLK_GATING_REG_t;


// 以下来自于 Allwinner H3 数据手册，p119的 DE_CLK_REG Register

// DE 时钟寄存器
typedef struct {
    uint32_t CLK_DIV_RATIO_M : 4;  // [0:3] 时钟分频因子 M (分频比 = M+1), 取值范围 1~16
    uint32_t RESERVED4_23    : 20; // [4:23] 保留
    uint32_t CLK_SRC_SEL     : 3;  // [24:26] 时钟源选择 (000: PLL_PERIPH0(2X), 001: PLL_DE, 其他: /)
    uint32_t RESERVED27_30   : 4;  // [27:30] 保留
    uint32_t SCLK_GATING     : 1;  // [31] 特殊时钟门控 (0: Clock is OFF, 1: Clock is ON)
} DE_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p120的 TCON0_CLK_REG Register

// TCON0 时钟寄存器
typedef struct {
    uint32_t CLK_DIV_RATIO_M : 4;  // [0:3] 时钟分频因子 M (分频比 = M+1), 取值范围 1~16
    uint32_t RESERVED4_23    : 20; // [4:23] 保留
    uint32_t CLK_SRC_SEL     : 3;  // [24:26] 时钟源选择 (000: PLL_VIDEO, 其他: /)
    uint32_t RESERVED27_30   : 4;  // [27:30] 保留
    uint32_t SCLK_GATING     : 1;  // [31] 特殊时钟门控 (0: Clock is OFF, 1: Clock is ON)
} TCON0_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p120的 TVE_CLK_REG Register

// TVE 时钟寄存器
typedef struct {
    uint32_t CLK_DIV_RATIO_M : 4;  // [0:3] 时钟分频因子 M (分频比 = M+1), 取值范围 1~16
    uint32_t RESERVED4_23    : 20; // [4:23] 保留
    uint32_t CLK_SRC_SEL     : 3;  // [24:26] 时钟源选择 (000: PLL_DE, 001: PLL_PERIPH1, 其他: /)
    uint32_t RESERVED27_30   : 4;  // [27:30] 保留
    uint32_t SCLK_GATING     : 1;  // [31] 特殊时钟门控 (0: Clock is OFF, 1: Clock is ON)
} TVE_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p121的 DEINTERLACE_CLK_REG Register

// DEINTERLACE 时钟寄存器
typedef struct {
    uint32_t CLK_DIV_RATIO_M : 4;  // [0:3] 时钟分频因子 M (分频比 = M+1), 取值范围 1~16
    uint32_t RESERVED4_23    : 20; // [4:23] 保留
    uint32_t CLK_SRC_SEL     : 3;  // [24:26] 时钟源选择 (000: PLL_PERIPH0, 001: PLL_PERIPH1, 其他: /)
    uint32_t RESERVED27_30   : 4;  // [27:30] 保留
    uint32_t SCLK_GATING     : 1;  // [31] 特殊时钟门控 (0: Clock is OFF, 1: Clock is ON)
} DEINTERLACE_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p121的 CSI_MISC_CLK_REG Register

// CSI_MISC 时钟寄存器
typedef struct {
    uint32_t RESERVED0_30    : 31; // [0:30] 保留
    uint32_t MIPI_CSI_CFG    : 1;  // [31] MIPI CSI 配置时钟门控 (0: Clock is OFF, 1: Clock is ON，时钟为 OSC24M)
} CSI_MISC_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p121-122的 CSI_CLK_REG Register

// CSI 时钟寄存器
typedef struct {
    uint32_t CSI_MCLK_DIV_M  : 5;  // [0:4] CSI 主时钟分频因子 M (分频比 = M+1), 取值范围 1~32
    uint32_t RESERVED5_7     : 3;  // [5:7] 保留
    uint32_t MCLK_SRC_SEL    : 3;  // [8:10] 主时钟源选择 (000: OSC24M, 001: PLL_VIDEO, 010: PLL_PERIPH1, 其他: /)
    uint32_t RESERVED11_14   : 4;  // [11:14] 保留
    uint32_t CSI_MCLK_GATING : 1;  // [15] 主时钟门控 (0: Clock is OFF, 1: Clock is ON)
    uint32_t CSI_SCLK_DIV_M  : 4;  // [16:19] CSI 特殊时钟分频因子 M (分频比 = M+1), 取值范围 1~16
    uint32_t RESERVED20_23   : 4;  // [20:23] 保留
    uint32_t SCLK_SRC_SEL    : 3;  // [24:26] 特殊时钟源选择 (000: PLL_PERIPH0, 001: PLL_PERIPH1, 其他: /)
    uint32_t RESERVED27_30   : 4;  // [27:30] 保留
    uint32_t CSI_SCLK_GATING : 1;  // [31] 特殊时钟门控 (0: Clock is OFF, 1: Clock is ON)
} CSI_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p122的 VE_CLK_REG Register

// VE 时钟寄存器
typedef struct {
    uint32_t RESERVED0_15    : 16; // [0:15] 保留
    uint32_t CLK_DIV_RATIO_N : 3;  // [16:18] 时钟预分频因子 N (分频比 = N+1), 取值范围 1~8
    uint32_t RESERVED19_30   : 12; // [19:30] 保留
    uint32_t VE_SCLK_GATING  : 1;  // [31] VE 特殊时钟门控 (0: Clock is OFF, 1: Clock is ON，SCLK = PLL_VE / Divider N)
} VE_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p123的 AC_DIG_CLK_REG Register

// AC Digital 时钟寄存器
typedef struct {
    uint32_t RESERVED0_30    : 31; // [0:30] 保留
    uint32_t SCLK_1X_GATING  : 1;  // [31] 特殊时钟门控 (0: Clock is OFF, 1: Clock is ON，SCLK = PLL_AUDIO Output)
} AC_DIG_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p123的 AVS_CLK_REG Register

// AVS 时钟寄存器
typedef struct {
    uint32_t RESERVED0_30    : 31; // [0:30] 保留
    uint32_t SCLK_GATING     : 1;  // [31] 特殊时钟门控 (0: Clock is OFF, 1: Clock is ON，SCLK = OSC24M)
} AVS_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p123的 HDMI_CLK_REG Register

// HDMI 时钟寄存器
typedef struct {
    uint32_t CLK_DIV_RATIO_M : 4;  // [0:3] 时钟分频因子 M (分频比 = M+1), 取值范围 1~16
    uint32_t RESERVED4_23    : 20; // [4:23] 保留
    uint32_t SCLK_SEL        : 2;  // [24:25] 特殊时钟源选择 (00: PLL_VIDEO, 其他: /)
    uint32_t RESERVED26_30   : 5;  // [26:30] 保留
    uint32_t SCLK_GATING     : 1;  // [31] 特殊时钟门控 (0: Clock is OFF, 1: Clock is ON)
} HDMI_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p124的 HDMI_SLOW_CLK_REG Register

// HDMI 慢时钟寄存器
typedef struct {
    uint32_t RESERVED0_30         : 31; // [0:30] 保留
    uint32_t HDMI_DDC_CLK_GATING  : 1;  // [31] HDMI DDC 时钟门控 (0: Clock is OFF, 1: Clock is ON，SCLK = OSC24M)
} HDMI_SLOW_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p124的 MBUS_CLK_REG Register

// MBUS 时钟寄存器
typedef struct {
    uint32_t MBUS_SCLK_RATIO_M   : 3;  // [0:2] MBUS 时钟分频因子 M (分频比 = M+1), 取值范围 1~8
    uint32_t RESERVED3_23        : 21; // [3:23] 保留
    uint32_t MBUS_SCLK_SRC       : 2;  // [24:25] 时钟源选择 (00: OSC24M, 01: PLL_PERIPH0(2X), 10: PLL_DDR, 11: /)
    uint32_t RESERVED26_30       : 5;  // [26:30] 保留
    uint32_t MBUS_SCLK_GATING    : 1;  // [31] MBUS 时钟门控 (0: Clock is OFF, 1: Clock is ON)
} MBUS_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p124的 GPU_CLK_REG Register

// GPU 时钟寄存器
typedef struct {
    uint32_t CLK_DIV_RATIO_N : 3;  // [0:2] 时钟预分频因子 N (分频比 = N+1), 取值范围 1~8
    uint32_t RESERVED3_30    : 28; // [3:30] 保留
    uint32_t SCLK_GATING     : 1;  // [31] 特殊时钟门控 (0: Clock is OFF, 1: Clock is ON，SCLK = PLL_GPU / Divider N)
} GPU_CLK_REG_t;


// 以下来自于 Allwinner H3 数据手册，p125的 PLL_STABLE_TIME_REG0 Register

// PLL 稳定时间寄存器0
typedef struct {
    uint32_t PLL_LOCK_TIME   : 16; // [0:15] PLL 锁定时间，单位 us（除 PLL_CPU 外的 PLL 启用或修改后，经过该稳定时间对应锁定位会置位）
    uint32_t RESERVED16_31   : 16; // [16:31] 保留
} PLL_STABLE_TIME_REG0_t;


// 以下来自于 Allwinner H3 数据手册，p125的 PLL_STABLE_TIME_REG1 Register

// PLL 稳定时间寄存器1
typedef struct {
    uint32_t PLL_CPU_LOCK_TIME : 16; // [0:15] PLL_CPU 锁定时间，单位 us（PLL_CPU 启用或修改后，经过该稳定时间对应锁定位会置位）
    uint32_t RESERVED16_31     : 16; // [16:31] 保留
} PLL_STABLE_TIME_REG1_t;


// 以下来自于 Allwinner H3 数据手册，p125-126的 PLL_CPUX_BIAS_REG Register

// PLL_CPUX 偏置寄存器
typedef struct {
    uint32_t PLL_DAMP_FACT_CTRL : 4;  // [0:3] PLL 阻尼因子控制 [3:0]
    uint32_t RESERVED4_7        : 4;  // [4:7] 保留
    uint32_t PLL_LOCK_CTRL      : 3;  // [8:10] PLL 锁定时间控制 [2:0]
    uint32_t RESERVED11_15      : 5;  // [11:15] 保留
    uint32_t PLL_BIAS_CUR_CTRL  : 5;  // [16:20] PLL 偏置电流控制 [4:0]
    uint32_t RESERVED21_23      : 3;  // [21:23] 保留
    uint32_t PLL_VCO_BIAS_CTRL  : 4;  // [24:27] PLL VCO 偏置控制 [3:0]
    uint32_t EXG_MODE           : 1;  // [28] 交换模式 (CPU PLL 时钟源改选 PLL_PERIPH0 而不是 PLL_CPU)
    uint32_t RESERVED29_30      : 2;  // [29:30] 保留
    uint32_t VCO_RST            : 1;  // [31] VCO 复位输入
} PLL_CPUX_BIAS_REG_t;


// 以下来自于 Allwinner H3 数据手册，p126的 PLL_AUDIO_BIAS_REG Register

// PLL_AUDIO 偏置寄存器
typedef struct {
    uint32_t RESERVED0_15    : 16; // [0:15] 保留
    uint32_t PLL_BIAS_CUR    : 5;  // [16:20] PLL 偏置电流 [4:0]
    uint32_t RESERVED21_23   : 3;  // [21:23] 保留
    uint32_t PLL_VCO_BIAS    : 5;  // [24:28] PLL VCO 偏置电流 [4:0]
    uint32_t RESERVED29_31   : 3;  // [29:31] 保留
} PLL_AUDIO_BIAS_REG_t;


// 以下来自于 Allwinner H3 数据手册，p126的 PLL_VIDEO_BIAS_REG Register

// PLL_VIDEO 偏置寄存器
typedef struct {
    uint32_t PLL_DAMP_FACTOR_CTRL : 3;  // [0:2] PLL 阻尼因子控制 [2:0]
    uint32_t RESERVED3_15         : 13; // [3:15] 保留
    uint32_t PLL_BIAS_CTRL        : 5;  // [16:20] PLL 偏置控制 [4:0]
    uint32_t RESERVED21_23        : 3;  // [21:23] 保留
    uint32_t PLL_VCO_BIAS_CTRL    : 5;  // [24:28] PLL VCO 偏置控制 [4:0]
    uint32_t RESERVED29_31        : 3;  // [29:31] 保留
} PLL_VIDEO_BIAS_REG_t;


// 以下来自于 Allwinner H3 数据手册，p126-127的 PLL_VE_BIAS_REG Register

// PLL_VE 偏置寄存器
typedef struct {
    uint32_t PLL_DAMP_FACTOR_CTRL : 3;  // [0:2] PLL 阻尼因子控制 [2:0]
    uint32_t RESERVED3_15         : 13; // [3:15] 保留
    uint32_t PLL_BIAS_CTRL        : 5;  // [16:20] PLL 偏置控制 [4:0]
    uint32_t RESERVED21_23        : 3;  // [21:23] 保留
    uint32_t PLL_VCO_BIAS_CTRL    : 5;  // [24:28] PLL VCO 偏置控制 [4:0]
    uint32_t RESERVED29_31        : 3;  // [29:31] 保留
} PLL_VE_BIAS_REG_t;


// 以下来自于 Allwinner H3 数据手册，p127的 PLL_DDR_BIAS_REG Register

// PLL_DDR 偏置寄存器
typedef struct {
    uint32_t PLL_DAMP_FACTOR_CTRL   : 4;  // [0:3] PLL 阻尼因子控制 [3:0]
    uint32_t RESERVED4_11           : 8;  // [4:11] 保留
    uint32_t PLL_VCO_GAIN_CTRL      : 3;  // [12:14] PLL VCO 增益控制位 [2:0]
    uint32_t RESERVED15             : 1;  // [15] 保留
    uint32_t PLL_BIAS_CUR_CTRL      : 5;  // [16:20] PLL 偏置电流控制
    uint32_t RESERVED21_23          : 3;  // [21:23] 保留
    uint32_t PLL_BANDW_CTRL         : 1;  // [24] PLL 带宽控制 (0: Narrow, 1: Wide)
    uint32_t PLL_VCO_GAIN_CTRL_EN   : 1;  // [25] PLL VCO 增益控制使能 (0: Disable, 1: Enable)
    uint32_t RESERVED26_27          : 2;  // [26:27] 保留
    uint32_t PLL_VCO_BIAS           : 4;  // [28:31] PLL VCO 偏置 [3:0]
} PLL_DDR_BIAS_REG_t;


// 以下来自于 Allwinner H3 数据手册，p127-128的 PLL_PERIPH0_BIAS_REG Register

// PLL_PERIPH0 偏置寄存器
typedef struct {
    uint32_t PLL_DAMP_FACTOR_CTRL : 2;  // [0:1] PLL 阻尼因子控制 [1:0]
    uint32_t RESERVED2_3          : 2;  // [2:3] 保留
    uint32_t PLL_BANDW_CTRL       : 1;  // [4] PLL 带宽控制 (0: Narrow, 1: Wide)
    uint32_t RESERVED5_15         : 11; // [5:15] 保留
    uint32_t PLL_BIAS_CUR_CTRL    : 5;  // [16:20] PLL 偏置电流控制
    uint32_t RESERVED21_23        : 3;  // [21:23] 保留
    uint32_t PLL_VCO_BIAS         : 5;  // [24:28] PLL VCO 偏置 [4:0]
    uint32_t RESERVED29_31        : 3;  // [29:31] 保留
} PLL_PERIPH0_BIAS_REG_t;


// 以下来自于 Allwinner H3 数据手册，p128的 PLL_GPU_BIAS_REG Register

// PLL_GPU 偏置寄存器
typedef struct {
    uint32_t PLL_DAMP_FACTOR_CTRL : 3;  // [0:2] PLL 阻尼因子控制 [2:0]
    uint32_t RESERVED3_15         : 13; // [3:15] 保留
    uint32_t PLL_BIAS_CTRL        : 5;  // [16:20] PLL 偏置控制 [4:0]
    uint32_t RESERVED21_23        : 3;  // [21:23] 保留
    uint32_t PLL_VCO_BIAS_CTRL    : 5;  // [24:28] PLL VCO 偏置控制 [4:0]
    uint32_t RESERVED29_31        : 3;  // [29:31] 保留
} PLL_GPU_BIAS_REG_t;


// 以下来自于 Allwinner H3 数据手册，p128-129的 PLL_PERIPH1_BIAS_REG Register

// PLL_PERIPH1 偏置寄存器
typedef struct {
    uint32_t PLL_DAMP_FACTOR_CTRL : 2;  // [0:1] PLL 阻尼因子控制 [1:0]
    uint32_t RESERVED2_3          : 2;  // [2:3] 保留
    uint32_t PLL_BANDW_CTRL       : 1;  // [4] PLL 带宽控制 (0: Narrow, 1: Wide)
    uint32_t RESERVED5_15         : 11; // [5:15] 保留
    uint32_t PLL_BIAS_CUR_CTRL    : 5;  // [16:20] PLL 偏置电流控制
    uint32_t RESERVED21_23        : 3;  // [21:23] 保留
    uint32_t PLL_VCO_BIAS         : 5;  // [24:28] PLL VCO 偏置 [4:0]
    uint32_t RESERVED29_31        : 3;  // [29:31] 保留
} PLL_PERIPH1_BIAS_REG_t;


// 以下来自于 Allwinner H3 数据手册，p129的 PLL_DE_BIAS_REG Register

// PLL_DE 偏置寄存器
typedef struct {
    uint32_t PLL_DAMP_FACTOR_CTRL : 3;  // [0:2] PLL 阻尼因子控制 [2:0]
    uint32_t RESERVED3_15         : 13; // [3:15] 保留
    uint32_t PLL_BIAS_CTRL        : 5;  // [16:20] PLL 偏置控制 [4:0]
    uint32_t RESERVED21_23        : 3;  // [21:23] 保留
    uint32_t PLL_VCO_BIAS_CTRL    : 5;  // [24:28] PLL VCO 偏置控制 [4:0]
    uint32_t RESERVED29_31        : 3;  // [29:31] 保留
} PLL_DE_BIAS_REG_t;


// 以下来自于 Allwinner H3 数据手册，p129的 PLL_CPUX_TUN_REG Register

// PLL_CPUX 调谐寄存器
typedef struct {
    uint32_t C_B_OUT             : 7;  // [0:6] C-B-Out[6:0]，只读校验值
    uint32_t C_OD1               : 1;  // [7] C-Reg-Od1 For Verify
    uint32_t C_B_IN              : 7;  // [8:14] C-B-In[6:0] For Verify
    uint32_t C_OD                : 1;  // [15] C-Reg-Od For Verify
    uint32_t PLL_INIT_FREQ_CTRL  : 7;  // [16:22] PLL 初始频率控制 [6:0]
    uint32_t VCO_GAIN_CTRL       : 3;  // [23:25] VCO 增益控制位 [2:0]
    uint32_t VCO_GAIN_CTRL_EN    : 1;  // [26] VCO 增益控制使能 (0: Disable, 1: Enable)
    uint32_t PLL_BAND_WID_CTRL   : 1;  // [27] PLL 带宽控制 (0: Narrow, 1: Wide)
    uint32_t RESERVED28_31       : 4;  // [28:31] 保留
} PLL_CPUX_TUN_REG_t;


// 以下来自于 Allwinner H3 数据手册，p130的 PLL_DDR_TUN_REG Register

// PLL_DDR 调谐寄存器
typedef struct {
    uint32_t B_OUT               : 7;  // [0:6] B-Out[6:0]，只读校验值
    uint32_t OD                  : 1;  // [7] Reg-Od For Verify
    uint32_t B_IN                : 7;  // [8:14] B-In[6:0] For Verify
    uint32_t OD1                 : 1;  // [15] Reg-Od1 For Verify
    uint32_t PLL_INIT_FREQ_CTRL  : 7;  // [16:22] PLL 初始频率控制 [6:0]
    uint32_t VCO_RST             : 1;  // [23] VCO Reset In
    uint32_t PLL_LTIME_CTRL      : 3;  // [24:26] PLL 锁定时间控制 [2:0]
    uint32_t RESERVED27          : 1;  // [27] 保留
    uint32_t VREG1_OUT_EN        : 1;  // [28] Vreg1 输出使能 (0: Disable, 1: Enable)
    uint32_t RESERVED29_31       : 3;  // [29:31] 保留
} PLL_DDR_TUN_REG_t;


// 以下来自于 Allwinner H3 数据手册，p130-131的 PLL_CPUX_PAT_CTRL_REG Register

// PLL_CPUX Pattern 控制寄存器
typedef struct {
    uint32_t WAVE_BOT           : 17; // [0:16] 波形底值
    uint32_t FREQ               : 2;  // [17:18] 调制频率 (00: 31.5KHz, 01: 32KHz, 10: 32.5KHz, 11: 33KHz)
    uint32_t RESERVED19         : 1;  // [19] 保留
    uint32_t WAVE_STEP          : 9;  // [20:28] 波形步进值
    uint32_t SPR_FREQ_MODE      : 2;  // [29:30] 展频模式 (00: DC=0, 01: DC=1, 1X: Triangular)
    uint32_t SIG_DELT_PAT_EN    : 1;  // [31] Sigma-delta Pattern 使能
} PLL_CPUX_PAT_CTRL_REG_t;

// 以下来自于 Allwinner H3 数据手册，p131的 PLL_AUDIO_PAT_CTRL_REG Register

// PLL_AUDIO Pattern 控制寄存器
typedef struct {
    uint32_t WAVE_BOT        : 17; // [0:16] 波形底值
    uint32_t FREQ            : 2;  // [17:18] 调制频率 (00: 31.5KHz, 01: 32KHz, 10: 32.5KHz, 11: 33KHz)
    uint32_t RESERVED19      : 1;  // [19] 保留
    uint32_t WAVE_STEP       : 9;  // [20:28] 波形步进值
    uint32_t SPR_FREQ_MODE   : 2;  // [29:30] 展频模式 (00: DC=0, 01: DC=1, 1X: Triangular)
    uint32_t SIG_DELT_PAT_EN : 1;  // [31] Sigma-delta Pattern 使能
} PLL_AUDIO_PAT_CTRL_REG_t;


// 以下来自于 Allwinner H3 数据手册，p131-132的 PLL_VIDEO_PAT_CTRL_REG Register

// PLL_VIDEO Pattern 控制寄存器
typedef struct {
    uint32_t WAVE_BOT        : 17; // [0:16] 波形底值
    uint32_t FREQ            : 2;  // [17:18] 调制频率 (00: 31.5KHz, 01: 32KHz, 10: 32.5KHz, 11: 33KHz)
    uint32_t RESERVED19      : 1;  // [19] 保留
    uint32_t WAVE_STEP       : 9;  // [20:28] 波形步进值
    uint32_t SPR_FREQ_MODE   : 2;  // [29:30] 展频模式 (00: DC=0, 01: DC=1, 1X: Triangular)
    uint32_t SIG_DELT_PAT_EN : 1;  // [31] Sigma-delta Pattern 使能
} PLL_VIDEO_PAT_CTRL_REG_t;


// 以下来自于 Allwinner H3 数据手册，p132的 PLL_VE_PAT_CTRL_REG Register

// PLL_VE Pattern 控制寄存器
typedef struct {
    uint32_t WAVE_BOT        : 17; // [0:16] 波形底值
    uint32_t FREQ            : 2;  // [17:18] 调制频率 (00: 31.5KHz, 01: 32KHz, 10: 32.5KHz, 11: 33KHz)
    uint32_t RESERVED19      : 1;  // [19] 保留
    uint32_t WAVE_STEP       : 9;  // [20:28] 波形步进值
    uint32_t SPR_FREQ_MODE   : 2;  // [29:30] 展频模式 (00: DC=0, 01: DC=1, 1X: Triangular)
    uint32_t SIG_DELT_PAT_EN : 1;  // [31] Sigma-delta Pattern 使能
} PLL_VE_PAT_CTRL_REG_t;


// 以下来自于 Allwinner H3 数据手册，p132-133的 PLL_DDR_PAT_CTRL_REG Register

// PLL_DDR Pattern 控制寄存器
typedef struct {
    uint32_t WAVE_BOT        : 17; // [0:16] 波形底值
    uint32_t FREQ            : 2;  // [17:18] 调制频率 (00: 31.5KHz, 01: 32KHz, 10: 32.5KHz, 11: 33KHz)
    uint32_t RESERVED19      : 1;  // [19] 保留
    uint32_t WAVE_STEP       : 9;  // [20:28] 波形步进值
    uint32_t SPR_FREQ_MODE   : 2;  // [29:30] 展频模式 (00: DC=0, 01: DC=1, 1X: Triangular)
    uint32_t SIG_DELT_PAT_EN : 1;  // [31] Sigma-delta Pattern 使能
} PLL_DDR_PAT_CTRL_REG_t;


// 以下来自于 Allwinner H3 数据手册，p133的 PLL_GPU_PAT_CTRL_REG Register

// PLL_GPU Pattern 控制寄存器
typedef struct {
    uint32_t WAVE_BOT        : 17; // [0:16] 波形底值
    uint32_t FREQ            : 2;  // [17:18] 调制频率 (00: 31.5KHz, 01: 32KHz, 10: 32.5KHz, 11: 33KHz)
    uint32_t RESERVED19      : 1;  // [19] 保留
    uint32_t WAVE_STEP       : 9;  // [20:28] 波形步进值
    uint32_t SPR_FREQ_MODE   : 2;  // [29:30] 展频模式 (00: DC=0, 01: DC=1, 1X: Triangular)
    uint32_t SIG_DELT_PAT_EN : 1;  // [31] Sigma-delta Pattern 使能
} PLL_GPU_PAT_CTRL_REG_t;


// 以下来自于 Allwinner H3 数据手册，p133-134的 PLL_PERIPH1_PAT_CTRL_REG Register

// PLL_PERIPH1 Pattern 控制寄存器
typedef struct {
    uint32_t WAVE_BOT        : 17; // [0:16] 波形底值
    uint32_t FREQ            : 2;  // [17:18] 调制频率 (00: 31.5KHz, 01: 32KHz, 10: 32.5KHz, 11: 33KHz)
    uint32_t RESERVED19      : 1;  // [19] 保留
    uint32_t WAVE_STEP       : 9;  // [20:28] 波形步进值
    uint32_t SPR_FREQ_MODE   : 2;  // [29:30] 展频模式 (00: DC=0, 01: DC=1, 1X: Triangular)
    uint32_t SIG_DELT_PAT_EN : 1;  // [31] Sigma-delta Pattern 使能
} PLL_PERIPH1_PAT_CTRL_REG_t;


// 以下来自于 Allwinner H3 数据手册，p134的 PLL_DE_PAT_CTRL_REG Register

// PLL_DE Pattern 控制寄存器
typedef struct {
    uint32_t WAVE_BOT        : 17; // [0:16] 波形底值
    uint32_t FREQ            : 2;  // [17:18] 调制频率 (00: 31.5KHz, 01: 32KHz, 10: 32.5KHz, 11: 33KHz)
    uint32_t RESERVED19      : 1;  // [19] 保留
    uint32_t WAVE_STEP       : 9;  // [20:28] 波形步进值
    uint32_t SPR_FREQ_MODE   : 2;  // [29:30] 展频模式 (00: DC=0, 01: DC=1, 1X: Triangular)
    uint32_t SIG_DELT_PAT_EN : 1;  // [31] Sigma-delta Pattern 使能
} PLL_DE_PAT_CTRL_REG_t;


// 以下来自于 Allwinner H3 数据手册，p134-136的 BUS_SOFT_RST_REG0 Register

// 总线软复位寄存器 0
typedef struct {
    uint32_t RESERVED0_4         : 5; // [0:4] 保留
    uint32_t CE_RST              : 1; // [5] CE 复位 (0: Assert, 1: De-assert)
    uint32_t DMA_RST             : 1; // [6] DMA 复位 (0: Assert, 1: De-assert)
    uint32_t RESERVED7           : 1; // [7] 保留
    uint32_t SD0_RST             : 1; // [8] SD/MMC0 复位 (0: Assert, 1: De-assert)
    uint32_t SD1_RST             : 1; // [9] SD/MMC1 复位 (0: Assert, 1: De-assert)
    uint32_t SD2_RST             : 1; // [10] SD/MMC2 复位 (0: Assert, 1: De-assert)
    uint32_t RESERVED11_12       : 2; // [11:12] 保留
    uint32_t NAND_RST            : 1; // [13] NAND 复位 (0: Assert, 1: De-assert)
    uint32_t SDRAM_RST           : 1; // [14] SDRAM AHB 复位 (0: Assert, 1: De-assert)
    uint32_t RESERVED15_16       : 2; // [15:16] 保留
    uint32_t EMAC_RST            : 1; // [17] EMAC 复位 (0: Assert, 1: De-assert)
    uint32_t TS_RST              : 1; // [18] TS 复位 (0: Assert, 1: De-assert)
    uint32_t HSTMR_RST           : 1; // [19] HSTMR 复位 (0: Assert, 1: De-assert)
    uint32_t SPI0_RST            : 1; // [20] SPI0 复位 (0: Assert, 1: De-assert)
    uint32_t SPI1_RST            : 1; // [21] SPI1 复位 (0: Assert, 1: De-assert)
    uint32_t RESERVED22          : 1; // [22] 保留
    uint32_t USB_OTG_DEVICE_RST  : 1; // [23] USB OTG_Device 复位 (0: Assert, 1: De-assert)
    uint32_t USB_OTG_EHCI0_RST   : 1; // [24] USB OTG_EHCI0 复位 (0: Assert, 1: De-assert)
    uint32_t USBEHCI1_RST        : 1; // [25] USB EHCI1 复位 (0: Assert, 1: De-assert)
    uint32_t USBEHCI2_RST        : 1; // [26] USB EHCI2 复位 (0: Assert, 1: De-assert)
    uint32_t USBEHCI3_RST        : 1; // [27] USB EHCI3 复位 (0: Assert, 1: De-assert)
    uint32_t USB_OTG_OHCI0_RST   : 1; // [28] USB OTG_OHCI0 复位 (0: Assert, 1: De-assert)
    uint32_t USBOHCI1_RST        : 1; // [29] USB OHCI1 复位 (0: Assert, 1: De-assert)
    uint32_t USBOHCI2_RST        : 1; // [30] USB OHCI2 复位 (0: Assert, 1: De-assert)
    uint32_t USBOHCI3_RST        : 1; // [31] USB OHCI3 复位 (0: Assert, 1: De-assert)
} BUS_SOFT_RST_REG0_t;


// 以下来自于 Allwinner H3 数据手册，p137-138的 BUS_SOFT_RST_REG1 Register

// 总线软复位寄存器 1
typedef struct {
    uint32_t VE_RST              : 1; // [0] VE 复位 (0: Assert, 1: De-assert)
    uint32_t RESERVED1_2         : 2; // [1:2] 保留
    uint32_t TCON0_RST           : 1; // [3] TCON0 复位 (0: Assert, 1: De-assert)
    uint32_t TCON1_RST           : 1; // [4] TCON1 复位 (0: Assert, 1: De-assert)
    uint32_t DEINTERLACE_RST     : 1; // [5] DEINTERLACE 复位 (0: Assert, 1: De-assert)
    uint32_t RESERVED6_7         : 2; // [6:7] 保留
    uint32_t CSI_RST             : 1; // [8] CSI 复位 (0: Assert, 1: De-assert)
    uint32_t TVE_RST             : 1; // [9] TVE 复位 (0: Assert, 1: De-assert)
    uint32_t HDMI0_RST           : 1; // [10] HDMI0 复位 (0: Assert, 1: De-assert)
    uint32_t HDMI1_RST           : 1; // [11] HDMI1 复位 (0: Assert, 1: De-assert)
    uint32_t DE_RST              : 1; // [12] DE 复位 (0: Assert, 1: De-assert)
    uint32_t RESERVED13_19       : 7; // [13:19] 保留
    uint32_t GPU_RST             : 1; // [20] GPU 复位 (0: Assert, 1: De-assert)
    uint32_t MSGBOX_RST          : 1; // [21] MSGBOX 复位 (0: Assert, 1: De-assert)
    uint32_t SPINLOCK_RST        : 1; // [22] SPINLOCK 复位 (0: Assert, 1: De-assert)
    uint32_t RESERVED23_30       : 8; // [23:30] 保留
    uint32_t DBGSYS_RST          : 1; // [31] DBGSYS 复位 (0: Assert, 1: De-assert)
} BUS_SOFT_RST_REG1_t;


// 以下来自于 Allwinner H3 数据手册，p138的 BUS_SOFT_RST_REG2 Register

// 总线软复位寄存器 2
typedef struct {
    uint32_t RESERVED0_1         : 2; // [0:1] 保留
    uint32_t EPHY_RST            : 1; // [2] EPHY 复位 (0: Assert, 1: De-assert)
    uint32_t RESERVED3_31        : 29; // [3:31] 保留
} BUS_SOFT_RST_REG2_t;


// 以下来自于 Allwinner H3 数据手册，p138-139的 BUS_SOFT_RST_REG3 Register

// 总线软复位寄存器 3
typedef struct {
    uint32_t AC_RST              : 1; // [0] AC 复位 (0: Assert, 1: De-assert)
    uint32_t OWA_RST             : 1; // [1] OWA 复位 (0: Assert, 1: De-assert)
    uint32_t RESERVED2_7         : 6; // [2:7] 保留
    uint32_t THS_RST             : 1; // [8] THS 复位 (0: Assert, 1: De-assert)
    uint32_t RESERVED9_11        : 3; // [9:11] 保留
    uint32_t I2S_PCM0_RST        : 1; // [12] I2S/PCM0 复位 (0: Assert, 1: De-assert)
    uint32_t I2S_PCM1_RST        : 1; // [13] I2S/PCM1 复位 (0: Assert, 1: De-assert)
    uint32_t I2S_PCM2_RST        : 1; // [14] I2S/PCM2 复位 (0: Assert, 1: De-assert)
    uint32_t RESERVED15_31       : 17; // [15:31] 保留
} BUS_SOFT_RST_REG3_t;


// 以下来自于 Allwinner H3 数据手册，p139-140的 BUS_SOFT_RST_REG4 Register

// 总线软复位寄存器 4
typedef struct {
    uint32_t TWI0_RST            : 1; // [0] TWI0 复位 (0: Assert, 1: De-assert)
    uint32_t TWI1_RST            : 1; // [1] TWI1 复位 (0: Assert, 1: De-assert)
    uint32_t TWI2_RST            : 1; // [2] TWI2 复位 (0: Assert, 1: De-assert)
    uint32_t RESERVED3_15        : 13; // [3:15] 保留
    uint32_t UART0_RST           : 1; // [16] UART0 复位 (0: Assert, 1: De-assert)
    uint32_t UART1_RST           : 1; // [17] UART1 复位 (0: Assert, 1: De-assert)
    uint32_t UART2_RST           : 1; // [18] UART2 复位 (0: Assert, 1: De-assert)
    uint32_t UART3_RST           : 1; // [19] UART3 复位 (0: Assert, 1: De-assert)
    uint32_t SCR_RST             : 1; // [20] SCR 复位 (0: Assert, 1: De-assert)
    uint32_t RESERVED21_31       : 11; // [21:31] 保留
} BUS_SOFT_RST_REG4_t;


// 以下来自于 Allwinner H3 数据手册，p140的 CCU_SEC_SWITCH_REG Register

// CCU 安全切换寄存器
typedef struct {
    uint32_t PLL_SEC             : 1; // [0] PLL 相关寄存器安全属性 (0: Secure, 1: Non-secure)
    uint32_t BUS_SEC             : 1; // [1] Bus 相关寄存器安全属性 (0: Secure, 1: Non-secure)
    uint32_t MBUS_SEC            : 1; // [2] MBUS 时钟相关寄存器安全属性 (0: Secure, 1: Non-secure)
    uint32_t RESERVED3_31        : 29; // [3:31] 保留
} CCU_SEC_SWITCH_REG_t;


// 以下来自于 Allwinner H3 数据手册，p140-141的 PS_CTRL_REG Register

// PS 控制寄存器
typedef struct {
    uint32_t MOD_EN              : 1; // [0] 模块使能 (0: Disable, 1: Enable)
    uint32_t TIME_DET            : 3; // [1:3] 检测时间配置 (000: 0.5/4us, 001: 0.5/2us, 010: 0.5/1us, 011: 0.5*2us, ..., 111: 0.5*2^5us)
    uint32_t OSC_SEL             : 2; // [4:5] 振荡器选择 (00: IDLE, 01: SVT, 10: LVT, 11: ULVT)
    uint32_t DLY_SEL             : 1; // [6] 延迟选择 (0: 1 Cycle, 1: 2 Cycles)
    uint32_t DET_FIN             : 1; // [7] 检测完成标志 (0: Unfinished, 1: Finished，写 1 清零)
    uint32_t RESERVED8_31        : 24; // [8:31] 保留
} PS_CTRL_REG_t;


// 以下来自于 Allwinner H3 数据手册，p141的 PS_CNT_REG Register

// PS 计数寄存器
typedef struct {
    uint32_t PS_CNT              : 16; // [0:15] PS 计数值
    uint32_t RESERVED16_31       : 16; // [16:31] 保留
} PS_CNT_REG_t;

#endif //HOS_CCU_DATASHEET_H
