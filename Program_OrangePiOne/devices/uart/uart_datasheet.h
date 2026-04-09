//
// Created by huangcheng on 2026/4/9.
//

#ifndef HOS_UART_DATASHEET_H
#define HOS_UART_DATASHEET_H

#include "../../lib/lib_kernel/lib_kernel.h"

// 这部分内容来自 Allwinner_H3_Datasheet_v1.2.pdf，page 466-480，UART Register List / Register Description

// Allwinner H3 UART 模块基地址
#define UART0_BASE_ADDR              0x01C28000  // UART0 基地址
#define UART1_BASE_ADDR              0x01C28400  // UART1 基地址
#define UART2_BASE_ADDR              0x01C28800  // UART2 基地址
#define UART3_BASE_ADDR              0x01C28C00  // UART3 基地址
#define R_UART_BASE_ADDR             0x01F02800  // R-UART 基地址

// UART 寄存器偏移
#define UART_RBR_REG_OFFSET          0x0000      // UART 接收缓冲寄存器
#define UART_THR_REG_OFFSET          0x0000      // UART 发送保持寄存器
#define UART_DLL_REG_OFFSET          0x0000      // UART 波特率分频锁存低寄存器
#define UART_DLH_REG_OFFSET          0x0004      // UART 波特率分频锁存高寄存器
#define UART_IER_REG_OFFSET          0x0004      // UART 中断使能寄存器
#define UART_IIR_REG_OFFSET          0x0008      // UART 中断标识寄存器
#define UART_FCR_REG_OFFSET          0x0008      // UART FIFO 控制寄存器
#define UART_LCR_REG_OFFSET          0x000C      // UART 线路控制寄存器
#define UART_MCR_REG_OFFSET          0x0010      // UART Modem 控制寄存器
#define UART_LSR_REG_OFFSET          0x0014      // UART 线路状态寄存器
#define UART_MSR_REG_OFFSET          0x0018      // UART Modem 状态寄存器
#define UART_SCH_REG_OFFSET          0x001C      // UART Scratch 寄存器
#define UART_USR_REG_OFFSET          0x007C      // UART 状态寄存器
#define UART_TFL_REG_OFFSET          0x0080      // UART 发送 FIFO 电平寄存器
#define UART_RFL_REG_OFFSET          0x0084      // UART 接收 FIFO 电平寄存器
#define UART_HALT_REG_OFFSET         0x00A4      // UART Halt TX 寄存器


// 以下来自于 Allwinner H3 数据手册，p466的 UART_RBR Register

// UART 接收缓冲寄存器
typedef struct {
    uint32_t RBR                  : 8;  // [0:7] 接收缓冲寄存器，串口输入端接收到的数据字节
    uint32_t RESERVED8_31         : 24; // [8:31] 保留
} UART_RBR_REG_t;


// 以下来自于 Allwinner H3 数据手册，p467的 UART_THR Register

// UART 发送保持寄存器
typedef struct {
    uint32_t THR                  : 8;  // [0:7] 发送保持寄存器，写入待发送的数据字节
    uint32_t RESERVED8_31         : 24; // [8:31] 保留
} UART_THR_REG_t;


// 以下来自于 Allwinner H3 数据手册，p467的 UART_DLL Register

// UART 波特率分频锁存低寄存器
typedef struct {
    uint32_t DLL                  : 8;  // [0:7] 16 位波特率分频值低 8 位
    uint32_t RESERVED8_31         : 24; // [8:31] 保留
} UART_DLL_REG_t;


// 以下来自于 Allwinner H3 数据手册，p468的 UART_DLH Register

// UART 波特率分频锁存高寄存器
typedef struct {
    uint32_t DLH                  : 8;  // [0:7] 16 位波特率分频值高 8 位
    uint32_t RESERVED8_31         : 24; // [8:31] 保留
} UART_DLH_REG_t;


// 以下来自于 Allwinner H3 数据手册，p468-469的 UART_IER Register

// UART 中断使能寄存器
typedef struct {
    uint32_t ERBFI                : 1;  // [0] 接收数据可用中断使能 (0: 禁止, 1: 使能)
    uint32_t ETBEI                : 1;  // [1] 发送保持寄存器空中断使能 (0: 禁止, 1: 使能)
    uint32_t ELSI                 : 1;  // [2] 接收线路状态中断使能 (0: 禁止, 1: 使能)
    uint32_t EDSSI                : 1;  // [3] Modem 状态中断使能 (0: 禁止, 1: 使能)
    uint32_t RESERVED4_6          : 3;  // [4:6] 保留
    uint32_t PTIME                : 1;  // [7] 可编程 THRE 中断模式使能 (0: 禁止, 1: 使能)
    uint32_t RESERVED8_31         : 24; // [8:31] 保留
} UART_IER_REG_t;


// 以下来自于 Allwinner H3 数据手册，p469-470的 UART_IIR Register

// UART 中断标识寄存器
typedef struct {
    uint32_t IID                  : 4;  // [0:3] 当前最高优先级挂起中断标识
                                        // 0000: Modem Status
                                        // 0001: 无中断挂起
                                        // 0010: THR Empty
                                        // 0100: Received Data Available
                                        // 0110: Receiver Line Status
                                        // 0111: Busy Detect
                                        // 1100: Character Timeout

    uint32_t RESERVED4_5          : 2;  // [4:5] 保留
    uint32_t FEFLAG               : 2;  // [6:7] FIFO 使能标志 (00: 禁用, 11: 使能)
    uint32_t RESERVED8_31         : 24; // [8:31] 保留
} UART_IIR_REG_t;


// 以下来自于 Allwinner H3 数据手册，p470-471的 UART_FCR Register

// UART FIFO 控制寄存器
typedef struct {
    uint32_t FIFOE                : 1;  // [0] FIFO 使能 (0: 禁用收发 FIFO, 1: 使能收发 FIFO)
    uint32_t RFIFOR               : 1;  // [1] 接收 FIFO 复位 (写 1 复位，硬件自清零)
    uint32_t XFIFOR               : 1;  // [2] 发送 FIFO 复位 (写 1 复位，硬件自清零)
    uint32_t DMAM                 : 1;  // [3] DMA 模式选择 (0: Mode 0, 1: Mode 1)
    uint32_t TFT                  : 2;  // [4:5] 发送 FIFO 空触发阈值
                                        // 00: FIFO 空
                                        // 01: FIFO 中有 2 个字符
                                        // 10: FIFO 1/4 满
                                        // 11: FIFO 1/2 满

    uint32_t RT                   : 2;  // [6:7] 接收 FIFO 触发阈值
                                        // 00: FIFO 中有 1 个字符
                                        // 01: FIFO 1/4 满
                                        // 10: FIFO 1/2 满
                                        // 11: FIFO 距离满仅差 2 个字符

    uint32_t RESERVED8_31         : 24; // [8:31] 保留
} UART_FCR_REG_t;


// 以下来自于 Allwinner H3 数据手册，p471-472的 UART_LCR Register

// UART 线路控制寄存器
typedef struct {
    uint32_t DLS                  : 2;  // [0:1] 数据位长度选择
                                        // 00: 5 bits
                                        // 01: 6 bits
                                        // 10: 7 bits
                                        // 11: 8 bits

    uint32_t STOP                 : 1;  // [2] 停止位长度选择
                                        // 0: 1 个停止位
                                        // 1: DLS=00 时为 1.5 个停止位，否则为 2 个停止位

    uint32_t PEN                  : 1;  // [3] 奇偶校验使能 (0: 禁用, 1: 使能)
    uint32_t EPS                  : 2;  // [4:5] 偶校验选择
                                        // 00: 奇校验
                                        // 01: 偶校验
                                        // 1X: 对 LCR[4] 取反

    uint32_t BC                   : 1;  // [6] Break 控制位 (1: 强制发送逻辑 0)
    uint32_t DLAB                 : 1;  // [7] 分频锁存访问使能
                                        // 0: 访问 RBR/THR 和 IER
                                        // 1: 访问 DLL 和 DLH

    uint32_t RESERVED8_31         : 24; // [8:31] 保留
} UART_LCR_REG_t;


// 以下来自于 Allwinner H3 数据手册，p473-474的 UART_MCR Register

// UART Modem 控制寄存器
typedef struct {
    uint32_t DTR                  : 1;  // [0] Data Terminal Ready 控制
                                        // 0: dtr_n 去使能 (逻辑 1)
                                        // 1: dtr_n 使能   (逻辑 0)

    uint32_t RTS                  : 1;  // [1] Request To Send 控制
                                        // 0: rts_n 去使能 (逻辑 1)
                                        // 1: rts_n 使能   (逻辑 0)

    uint32_t RESERVED2_3          : 2;  // [2:3] 保留
    uint32_t LOOP                 : 1;  // [4] 回环模式 (0: 正常模式, 1: 回环模式)
    uint32_t AFCE                 : 1;  // [5] 自动流控使能 (0: 禁用, 1: 使能)
    uint32_t RESERVED6_31         : 26; // [6:31] 保留
} UART_MCR_REG_t;


// 以下来自于 Allwinner H3 数据手册，p474-476的 UART_LSR Register

// UART 线路状态寄存器
typedef struct {
    uint32_t DR                   : 1;  // [0] 数据就绪 (0: 无数据, 1: 接收缓冲/FIFO 中至少有 1 个字符)
    uint32_t OE                   : 1;  // [1] 溢出错误 (0: 无, 1: 发生溢出)
    uint32_t PE                   : 1;  // [2] 奇偶校验错误 (0: 无, 1: 有)
    uint32_t FE                   : 1;  // [3] 帧错误 (0: 无, 1: 有)
    uint32_t BI                   : 1;  // [4] Break 中断指示 (0: 无, 1: 检测到 Break)
    uint32_t THRE                 : 1;  // [5] 发送保持寄存器空 (0: 非空, 1: 空)
    uint32_t TEMT                 : 1;  // [6] 发送器空 (0: 非空, 1: 空)
    uint32_t FIFOERR              : 1;  // [7] FIFO 中存在接收错误 (0: 无, 1: 有至少一个 PE/FE/BI)
    uint32_t RESERVED8_31         : 24; // [8:31] 保留
} UART_LSR_REG_t;


// 以下来自于 Allwinner H3 数据手册，p476-477的 UART_MSR Register

// UART Modem 状态寄存器
typedef struct {
    uint32_t DCTS                 : 1;  // [0] CTS 变化指示 (读 MSR 清零)
    uint32_t DDSR                 : 1;  // [1] DSR 变化指示 (读 MSR 清零)
    uint32_t TERI                 : 1;  // [2] RI 拖尾边沿指示 (读 MSR 清零)
    uint32_t DDCD                 : 1;  // [3] DCD 变化指示 (读 MSR 清零)
    uint32_t CTS                  : 1;  // [4] 当前 CTS 线路状态 (0: 去使能, 1: 使能)
    uint32_t DSR                  : 1;  // [5] 当前 DSR 线路状态 (0: 去使能, 1: 使能)
    uint32_t RI                   : 1;  // [6] 当前 RI 线路状态  (0: 去使能, 1: 使能)
    uint32_t DCD                  : 1;  // [7] 当前 DCD 线路状态 (0: 去使能, 1: 使能)
    uint32_t RESERVED8_31         : 24; // [8:31] 保留
} UART_MSR_REG_t;


// 以下来自于 Allwinner H3 数据手册，p477-478的 UART_SCH Register

// UART Scratch 寄存器
typedef struct {
    uint32_t SCRATCH_REG          : 8;  // [0:7] Scratch 临时存储寄存器，供软件临时使用
    uint32_t RESERVED8_31         : 24; // [8:31] 保留
} UART_SCH_REG_t;


// 以下来自于 Allwinner H3 数据手册，p478的 UART_USR Register

// UART 状态寄存器
typedef struct {
    uint32_t BUSY                 : 1;  // [0] UART Busy 标志 (0: Idle/Inactive, 1: Busy)
    uint32_t TFNF                 : 1;  // [1] 发送 FIFO 未满 (0: 满, 1: 未满)
    uint32_t TFE                  : 1;  // [2] 发送 FIFO 为空 (0: 非空, 1: 空)
    uint32_t RFNE                 : 1;  // [3] 接收 FIFO 非空 (0: 空, 1: 非空)
    uint32_t RFF                  : 1;  // [4] 接收 FIFO 满 (0: 未满, 1: 满)
    uint32_t RESERVED5_31         : 27; // [5:31] 保留
} UART_USR_REG_t;


// 以下来自于 Allwinner H3 数据手册，p478-479的 UART_TFL Register

// UART 发送 FIFO 电平寄存器
typedef struct {
    uint32_t TFL                  : 7;  // [0:6] 发送 FIFO 中当前数据项个数
    uint32_t RESERVED7_31         : 25; // [7:31] 保留
} UART_TFL_REG_t;


// 以下来自于 Allwinner H3 数据手册，p479的 UART_RFL Register

// UART 接收 FIFO 电平寄存器
typedef struct {
    uint32_t RFL                  : 7;  // [0:6] 接收 FIFO 中当前数据项个数
    uint32_t RESERVED7_31         : 25; // [7:31] 保留
} UART_RFL_REG_t;


// 以下来自于 Allwinner H3 数据手册，p479-480的 UART_HALT Register

// UART Halt TX 寄存器
typedef struct {
    uint32_t HALT_TX              : 1;  // [0] 停止发送 (0: 禁止 Halt TX, 1: 使能 Halt TX)
    uint32_t CHCFG_AT_BUSY        : 1;  // [1] UART Busy 时允许修改波特率和 LCR 配置 (0: 禁止, 1: 允许)
    uint32_t CHANGE_UPDATE        : 1;  // [2] 配置更新触发位 (写 1 触发更新，完成后硬件自清零)
    uint32_t RESERVED3            : 1;  // [3] 保留
    uint32_t SIR_TX_INVERT        : 1;  // [4] SIR 发送脉冲极性反转 (0: 不反转, 1: 反转)
    uint32_t SIR_RX_INVERT        : 1;  // [5] SIR 接收脉冲极性反转 (0: 不反转, 1: 反转)
    uint32_t RESERVED6_31         : 26; // [6:31] 保留
} UART_HALT_REG_t;

#endif //HOS_UART_DATASHEET_H
