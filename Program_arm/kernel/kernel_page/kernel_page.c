//
// Created by huangcheng on 2024/5/23.
//

#include "kernel_page.h"

// 注意，和x86不同的是，页目录表是4096条，需要16KB，也就是0x4000

void setup_page_directory() {

    SectionDescriptor *page_directory = (SectionDescriptor *)(PAGE_DIR_TABLE_POS + DRAM_OFFSET);

    // 清空整个页目录表（4096条，要16KB）
    memset(page_directory, 0, 0x4000);

    // 低端一对一映射（DRAM地址从0x40000000开始，不仅是内存，还有uart，还有可能的其他组件，因此全部用段描述符映射）
    // 0x40000000 / 0x100000 = 0x400
    // 0x48000000 / 0x100000 = 0x480
    for(uint32_t i = 0; i < 0x480; i++) {
        page_directory[i].DescriptorType = 2;
        page_directory[i].Bufferable = 1;
        page_directory[i].Cacheable = 1;
        page_directory[i].ShouldBeZero0 = 0;
        page_directory[i].Domain = USER_DOMAIN;
        page_directory[i].ImplementationDefined = 0;
        page_directory[i].AccessPermission = 3; // 用户域不检查，但还是以防万一
        page_directory[i].TypeExtension = 0;
        page_directory[i].ShouldBeZero1 = 0;
        page_directory[i].Shared = 0;
        page_directory[i].ShouldBeZero2 = 0;
        page_directory[i].PresentHigh = 0;
        page_directory[i].ShouldBeZero3 = 0;
        // page_directory[i].BaseAddress = ((i * 0x100000) >> 20);
        // 简化为：
        page_directory[i].BaseAddress = i;
    }


    // 1条页表放高端1MB映射
    // 从0xc0000000开始
    page_directory[0xc00].DescriptorType = 2;
    page_directory[0xc00].Bufferable = 1;
    page_directory[0xc00].Cacheable = 1;
    page_directory[0xc00].ShouldBeZero0 = 0;
    page_directory[0xc00].Domain = KERNEL_DOMAIN;
    page_directory[0xc00].ImplementationDefined = 0;
    page_directory[0xc00].AccessPermission = 2;     // 当然要特权读写用户只读
    page_directory[0xc00].TypeExtension = 0;
    page_directory[0xc00].ShouldBeZero1 = 0;
    page_directory[0xc00].Shared = 0;
    page_directory[0xc00].ShouldBeZero2 = 0;
    page_directory[0xc00].PresentHigh = 0;
    page_directory[0xc00].ShouldBeZero3 = 0;
    page_directory[0xc00].BaseAddress = (DRAM_OFFSET >> 20);


    // 增加自引用（偷懒所以用段描述符，实际上为了权限控制的话，应该用粗页表，把范围定死在这16KB里面）
    // 这样访问的地址就是0xfff00000
    // 但是注意，范围很大，有1MB，别越界
    page_directory[0xfff].DescriptorType = 2;
    page_directory[0xfff].Bufferable = 1;
    page_directory[0xfff].Cacheable = 1;
    page_directory[0xfff].ShouldBeZero0 = 0;
    page_directory[0xfff].Domain = KERNEL_DOMAIN;
    page_directory[0xfff].ImplementationDefined = 0;
    page_directory[0xfff].AccessPermission = 2;     // 当然要特权读写用户只读
    page_directory[0xfff].TypeExtension = 0;
    page_directory[0xfff].ShouldBeZero1 = 0;
    page_directory[0xfff].Shared = 0;
    page_directory[0xfff].ShouldBeZero2 = 0;
    page_directory[0xfff].PresentHigh = 0;
    page_directory[0xfff].ShouldBeZero3 = 0;
    page_directory[0xfff].BaseAddress = ((PAGE_DIR_TABLE_POS + DRAM_OFFSET) >> 20);

    // ramdisk暂时不管，用到了再说
}

void init_paging() {
    // 设置页目录表
    setup_page_directory();

    // 先禁用MMU
    // 获取当前SCTLR寄存器值
    SCTLR_t sctlr;
    get_sctlr(&sctlr);
    // 禁用MMU
    sctlr.M = 0; // 禁用MMU
    sctlr.A = 0; // 禁用对齐检查
    sctlr.C = 0; // 禁用数据缓存
    // 更新SCTLR寄存器
    set_sctlr(&sctlr);

    // 域控制器设置
    DACR_t dacr;
    memset(&dacr, 0, sizeof(DACR_t));
    // 实测，一旦开启权限检查
    // 用粗页表和小页表的二级映射体系直接无法写，只能读
    // 为了避免权限问题，只能一律全权
    dacr.domain0 = 3;
    dacr.domain15 = 3;
    set_dacr(&dacr);

    // 设置TTBCR寄存器，只使用TTBR0，类似于x86的cr3，兼容先前的ARMv5和ARMv6架构
    TTBCR_t ttbcr;
    // 取当前TTBCR值
    get_ttbcr(&ttbcr);
    ttbcr.N = 0;
    ttbcr.Reserved0 = 0;
    ttbcr.PD0 = 0;
    ttbcr.PD1 = 0;
    ttbcr.Reserved1 = 0;
    ttbcr.EAE = 0;
    // 设置TTBCR寄存器
    set_ttbcr(&ttbcr);

    // 载入页目录表
    load_page_directory(PAGE_DIR_TABLE_POS + DRAM_OFFSET);

    // 无效化TLB
    invalidate_tlbs();

    // 启用MMU
    get_sctlr(&sctlr);
    sctlr.M = 1; // 启用MMU
    sctlr.A = 1; // 启用对齐检查
    sctlr.C = 1; // 启用数据缓存
    // 更新SCTLR寄存器
    set_sctlr(&sctlr);
}
