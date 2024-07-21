//
// Created by huangcheng on 2024/5/23.
//

#ifndef HOS_KERNEL_PAGE_H
#define HOS_KERNEL_PAGE_H

#include "../../lib/lib_kernel/lib_kernel.h"

// JZ2440用的CPU是SAMSUNG S3C2410
// 架构是arm v4T架构，和arm v7架构很不一样
// 主要是粗页表和段页表的结构要改，而且arm v4T也没有超级段这种东西
// 其次是根本没有TTBCR，这部分完全可以取消掉

// 一级页表的资料
//https://developer.arm.com/documentation/ddi0151/c/memory-management-unit/address-translation/level-one-descriptor?lang=en

// 粗页表
typedef struct {
    uint32_t DescriptorType :2;            // 位0-1: 描述符类型，粗页表为1（二进制为01）
    uint32_t ShouldBeZero : 2;             // 位2-3: 必须为0
    uint32_t ShouldBeOne : 1;              // 位4: 必须为1
    uint32_t Domain : 4;                   // 位5-8: 域，用作内存保护机制，16个可能的域
    uint32_t ShouldBeZero2 : 1;            // 位9: 必须为0
    uint32_t BaseAddress : 22;             // 位10-31: 页表基地址，必须1KB对齐（地址右移10位）
} CoarsePageTableEntry;

// 段描述符
typedef struct {
    uint32_t DescriptorType :2;            // 位0-1: 描述符类型，段描述符为2（二进制为10）
    uint32_t Bufferable : 1;               // 位2: 缓冲位
    uint32_t Cacheable : 1;                // 位3: 缓存位
    uint32_t ShouldBeOne : 1;              // 位4: 必须为1
    uint32_t Domain : 4;                   // 位5-8: 域，用作内存保护机制，16个可能的域
    uint32_t ShouldBeZero : 1;             // 位9: 必须为0
    uint32_t AccessPermission : 2;         // 位10-11: 访问权限位1:0（0是禁止任何访问，1是特权读写，2是特权读写用户只读，3是全权）
    uint32_t ShouldBeZero2 : 8;            // 位12-19： 必须为0
    uint32_t BaseAddress : 12;             // 位20-31: 段基地址，必须1MB对齐（地址右移20位）
} SectionDescriptor;

// 关于AP：0是禁止任何访问，1是特权读写，2是特权读写用户只读，3是特权和用户都是全权

// 在 ARMv7 架构中，域 (Domain) 是一种访问控制机制，用于管理对内存的访问。
// 每个域包含一组页表条目，系统可以对这些域施加不同的访问控制策略。
// 域访问控制寄存器 (DACR) 用于控制每个域的访问权限。
// 每个域可以设置为以下三种状态之一：
// 1. No Access (无访问权限): 任何尝试访问该域的内存都会导致访问权限错误。
// 2. Client (客户端): 访问权限由页表条目的访问权限字段决定。
// 3. Manager (管理): 允许对该域的所有访问，无需检查页表条目的访问权限字段。
// 每个页目录项包含一个 4 位的域字段，这个字段用于指定页表条目所属的域。域的数量最多为 16 (0-15)。

// 域控制器用结构体位域的表示方法
typedef struct {
    uint32_t domain0  : 2;  // 0，二进制00，无访问权；1，二进制01，Client，由页表条目的访问权限字段决定；3，二进制11，Manager，允许对该域所有访问，完全不检查权限
    uint32_t domain1  : 2;  // 0，二进制00，无访问权；1，二进制01，Client，由页表条目的访问权限字段决定；3，二进制11，Manager，允许对该域所有访问，完全不检查权限
    uint32_t domain2  : 2;  // 0，二进制00，无访问权；1，二进制01，Client，由页表条目的访问权限字段决定；3，二进制11，Manager，允许对该域所有访问，完全不检查权限
    uint32_t domain3  : 2;  // 0，二进制00，无访问权；1，二进制01，Client，由页表条目的访问权限字段决定；3，二进制11，Manager，允许对该域所有访问，完全不检查权限
    uint32_t domain4  : 2;  // 0，二进制00，无访问权；1，二进制01，Client，由页表条目的访问权限字段决定；3，二进制11，Manager，允许对该域所有访问，完全不检查权限
    uint32_t domain5  : 2;  // 0，二进制00，无访问权；1，二进制01，Client，由页表条目的访问权限字段决定；3，二进制11，Manager，允许对该域所有访问，完全不检查权限
    uint32_t domain6  : 2;  // 0，二进制00，无访问权；1，二进制01，Client，由页表条目的访问权限字段决定；3，二进制11，Manager，允许对该域所有访问，完全不检查权限
    uint32_t domain7  : 2;  // 0，二进制00，无访问权；1，二进制01，Client，由页表条目的访问权限字段决定；3，二进制11，Manager，允许对该域所有访问，完全不检查权限
    uint32_t domain8  : 2;  // 0，二进制00，无访问权；1，二进制01，Client，由页表条目的访问权限字段决定；3，二进制11，Manager，允许对该域所有访问，完全不检查权限
    uint32_t domain9  : 2;  // 0，二进制00，无访问权；1，二进制01，Client，由页表条目的访问权限字段决定；3，二进制11，Manager，允许对该域所有访问，完全不检查权限
    uint32_t domain10 : 2;  // 0，二进制00，无访问权；1，二进制01，Client，由页表条目的访问权限字段决定；3，二进制11，Manager，允许对该域所有访问，完全不检查权限
    uint32_t domain11 : 2;  // 0，二进制00，无访问权；1，二进制01，Client，由页表条目的访问权限字段决定；3，二进制11，Manager，允许对该域所有访问，完全不检查权限
    uint32_t domain12 : 2;  // 0，二进制00，无访问权；1，二进制01，Client，由页表条目的访问权限字段决定；3，二进制11，Manager，允许对该域所有访问，完全不检查权限
    uint32_t domain13 : 2;  // 0，二进制00，无访问权；1，二进制01，Client，由页表条目的访问权限字段决定；3，二进制11，Manager，允许对该域所有访问，完全不检查权限
    uint32_t domain14 : 2;  // 0，二进制00，无访问权；1，二进制01，Client，由页表条目的访问权限字段决定；3，二进制11，Manager，允许对该域所有访问，完全不检查权限
    uint32_t domain15 : 2;  // 0，二进制00，无访问权；1，二进制01，Client，由页表条目的访问权限字段决定；3，二进制11，Manager，允许对该域所有访问，完全不检查权限
} DACR_t;

// 获取DACR寄存器的值并存储到结构体中，paging_ops.asm
extern void get_dacr(DACR_t *dacr);
// 从结构体中读取并设置DACR寄存器的值，paging_ops.asm
extern void set_dacr(DACR_t *dacr);

// ARM 二级页表（分大页、小页、细页三种，大页64KB，小页4KB，细页1KB，细页现在基本上不用了）

// 二级页表的资料
//https://developer.arm.com/documentation/ddi0151/c/memory-management-unit/address-translation/level-two-descriptor?lang=en

// 大页，64KB
typedef struct {
    uint32_t PageType : 2;                     // 位0-1: 页类型，大页为1（二进制为01）
    uint32_t Bufferable : 1;                   // 位2: 缓冲位
    uint32_t Cacheable : 1;                    // 位3: 缓存位
    uint32_t AccessPermission0 : 2;            // 位4-5: 第一个4KB的访问权限，以此类推
    uint32_t AccessPermission1 : 2;            // 位6-8: 从上类推
    uint32_t AccessPermission2 : 2;            // 位9-10: 从上类推
    uint32_t AccessPermission3 : 2;            // 位10-11: 从上类推
    uint32_t ShouldBeZero : 4;                 // 位12-15: 应为0
    uint32_t BaseAddress : 16;                 // 位16-31: 大页基地址，必须64KB对齐（地址右移16位）
} LargePageDescriptor;

// 小页，4KB
typedef struct {
    uint32_t PageType : 2;                     // 位0-1: 页类型，小页为2（二进制为10）
    uint32_t Bufferable : 1;                   // 位2: 缓冲位
    uint32_t Cacheable : 1;                    // 位3: 缓存位
    uint32_t AccessPermission0 : 2;            // 位4-5: 第一个1KB的访问权限，以此类推
    uint32_t AccessPermission1 : 2;            // 位6-8: 从上类推
    uint32_t AccessPermission2 : 2;            // 位9-10: 从上类推
    uint32_t AccessPermission3 : 2;            // 位10-11: 从上类推
    uint32_t BaseAddress : 20;                 // 位12-31: 小页基地址，必须4KB对齐（地址右移12位）
} SmallPageDescriptor;

// 这个看s3c24xx.h可以看到

#define DRAM_OFFSET 0x30000000          // 内存偏移地址

#define PAGE_DIR_TABLE_POS 0x100000     // 页目录表的起始物理地址

#define PAGE_TABLE_ENTRIES 256          // 每个页表中的页表项数量
#define PAGE_DIR_ENTRIES 4096           // 页目录中的页目录项数量

// Linux分三个域，我只分两个（域只是个编号，没有高低之分，区分开就行）
#define KERNEL_DOMAIN 0     // 内核域
#define USER_DOMAIN 15      // 用户域

// 定义SCTLR寄存器的结构体
typedef struct {
    unsigned int M : 1;             // 位0: 内存控制位：0表示禁用MMU或MPU，1表示启用
    unsigned int A : 1;             // 位1: 对齐检查使能位：0表示禁用对齐故障检查，1表示启用
    unsigned int C : 1;             // 位2: 缓存使能位：0表示禁用数据和统一缓存，1表示启用
    unsigned int W : 1;             // 位3: 写缓冲区使能位：0表示禁用写缓冲区，1表示启用
    unsigned int reserved1 : 3;     // 位4-6: 保留，RAO/SBOP
    unsigned int B : 1;             // 位7: 大小端配置位：0表示小端，1表示大端（BE-32）
    unsigned int S : 1;             // 位8: 系统保护位，向后兼容
    unsigned int R : 1;             // 位9: ROM保护位，向后兼容
    unsigned int F : 1;             // 位10: 实现定义
    unsigned int Z : 1;             // 位11: 分支预测使能位：0表示禁用，1表示启用
    unsigned int I : 1;             // 位12: 指令缓存使能位：0表示禁用，1表示启用
    unsigned int V : 1;             // 位13: 向量表基址选择位：0表示0x00000000，1表示0xFFFF0000
    unsigned int RR : 1;            // 位14: 轮替策略选择位：0表示普通替换策略，1表示可预测策略
    unsigned int L4 : 1;            // 位15: ARMv5T Thumb互操作行为抑制位
    unsigned int reserved2 : 16;    // 位16-31: 保留
} SCTLR_t;

// 获取SCTLR寄存器的值并存储到结构体中，paging_ops.asm
extern void get_sctlr(SCTLR_t *sctlr);
// 从结构体中读取并设置SCTLR寄存器的值，paging_ops.asm
extern void set_sctlr(SCTLR_t *sctlr);

// 刷新TLB，使TLB无效
extern void invalidate_tlbs();

// 内存分页功能初始化
void init_paging();


#endif //HOS_KERNEL_PAGE_H
