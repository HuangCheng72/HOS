.section .text

.global load_page_directory
.global get_current_page
.global get_sctlr
.global set_sctlr
.global get_dacr
.global set_dacr
.global invalidate_tlbs

// x86的C调用约定完全通过栈传参
// arm不一样，前四个参数通过r0到r3四个寄存器传参，多余的参数通过栈传参
// 第一个参数就是r0
// 所以直接加载就行了

// void load_page_directory(uint32_t page_directory_addr);
load_page_directory:
    // 设置TTBR
    mcr p15, 0, r0, c2, c0, 0    // 设置TTBR，r0包含页表基地址
    bx lr                        // 返回调用者

// uint32_t get_current_page(void);
get_current_page:
    mrc p15, 0, r0, c2, c0, 0    // 读取TTBR寄存器
    bx lr

// 获取SCTLR寄存器的值并存储到结构体中
// void get_sctlr(SCTLR_t *sctlr);
get_sctlr:
    // 读取SCTLR寄存器的值到r1
    mrc p15, 0, r1, c1, c0, 0
    // 将r1的值存储到结构体中
    str r1, [r0]
    // 返回
    bx lr

// 从结构体中读取并设置SCTLR寄存器的值
// void set_sctlr(SCTLR_t *sctlr);
set_sctlr:
    // 从结构体中加载值到r1
    ldr r1, [r0]
    // 将r1的值写入SCTLR寄存器
    mcr p15, 0, r1, c1, c0, 0
    // 返回
    bx lr

// 获取DACR寄存器的值并存储到结构体中
// void get_dacr(DACR_t *dacr);
get_dacr:
    // 读取DACR寄存器的值到r1
    mrc p15, 0, r1, c3, c0, 0
    // 将r1的值存储到结构体中
    str r1, [r0]
    // 返回
    bx lr

// 从结构体中读取并设置DACR寄存器的值
// void set_dacr(DACR_t *dacr);
set_dacr:
    // 从结构体中加载值到r1
    ldr r1, [r0]
    // 将r1的值写入DACR寄存器
    mcr p15, 0, r1, c3, c0, 0
    // 返回
    bx lr

// 刷新TLB，使TLB无效
invalidate_tlbs:
    mov r2, #0
    mcr p15, 0, r2, c8, c7, 0    // 使TLB无效
    mcr p15, 0, r2, c7, c10, 4   // 数据同步屏障（DSB）
    bx lr
