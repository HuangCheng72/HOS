//
// Created by huangcheng on 2024/5/23.
//

#include "kernel_page.h"

// 注意，和x86不同的是，页目录表是4096条，需要16KB，也就是0x4000

void setup_page_directory() {

    page_directory_entry_t *page_directory = (page_directory_entry_t *)(PAGE_DIR_TABLE_POS + DRAM_OFFSET);

    // 清空整个页目录表（4096条，要16KB）
    memset(page_directory, 0, 0x4000);

    // 一级页表用粗页表，二级页表用小页表
    // 因为0x9000000处是uart的映射，这里也需要做一对一映射

    // 4个页表放低端4MB映射
    page_table_entry_t* first_page_table = (page_table_entry_t*)(PAGE_DIR_TABLE_POS + DRAM_OFFSET + 0x4000);

    // 4MB就是0x00400000，则是0x400页，这么多个描述符
    for(uint32_t i = 0; i < 0x400; i++) {
        first_page_table[i].PageType = 2;
        first_page_table[i].Bufferable = 1;
        first_page_table[i].Cacheable = 1;
        // 低端的访问权应当是全权，不然用户怎么操作
        first_page_table[i].AccessPermission0 = 3;
        first_page_table[i].AccessPermission1 = 3;
        first_page_table[i].AccessPermission2 = 3;
        first_page_table[i].AccessPermission3 = 3;
        first_page_table[i].BaseAddress = ((DRAM_OFFSET + i * 0x1000) >> 12);
    }

    // 把四条一级页表赋值了
    // 256个二级页表是1024字节，也就是0x400
    // 低端一对一映射（DRAM地址从0x40000000开始）
    // 0x40000000 / 0x100000 = 0x400
    for(uint32_t i = 0; i < 4; i++) {
        page_directory[i + 0x400].DescriptorType = 1;
        page_directory[i + 0x400].ShouldBeZero = 0;
        page_directory[i + 0x400].Domain = USER_DOMAIN;
        page_directory[i + 0x400].ImplementationDefined = 0;
        page_directory[i + 0x400].BaseAddress = (((uint32_t)(first_page_table) + i * 0x400) >> 10);
    }


    // 1条页表放高端1MB映射
    page_table_entry_t* second_page_table = (page_table_entry_t*)(PAGE_DIR_TABLE_POS + DRAM_OFFSET + 0x4000 + 0x400 * sizeof(page_table_entry_t));

    // 1MB就是0x00100000，则是0x100页
    for(uint32_t i = 0; i < 0x100; i++) {
        second_page_table[i].PageType = 2;
        second_page_table[i].Bufferable = 1;
        second_page_table[i].Cacheable = 1;
        // 高端的访问权应当是特权读写、用户只读，所以用2合适
        second_page_table[i].AccessPermission0 = 2;
        second_page_table[i].AccessPermission1 = 2;
        second_page_table[i].AccessPermission2 = 2;
        second_page_table[i].AccessPermission3 = 2;
        second_page_table[i].BaseAddress = ((DRAM_OFFSET + i * 0x1000) >> 12);
    }

    // 从0xc0000000开始
    page_directory[0xc00].DescriptorType = 1;
    page_directory[0xc00].ShouldBeZero = 0;
    page_directory[0xc00].Domain = KERNEL_DOMAIN;
    page_directory[0xc00].ImplementationDefined = 0;
    page_directory[0xc00].BaseAddress = ((uint32_t)(second_page_table) >> 10);


    // 0x09000000处放一对一映射一页，要不然没法用串口输出
    page_table_entry_t* third_page_table = (page_table_entry_t*)(PAGE_DIR_TABLE_POS + DRAM_OFFSET + 0x4000 + 0x500 * sizeof(page_table_entry_t));

    third_page_table->PageType = 2;
    third_page_table->Bufferable = 1;
    third_page_table->Cacheable = 1;
    // 这里的访问权肯定要全开
    third_page_table->AccessPermission0 = 3;
    third_page_table->AccessPermission1 = 3;
    third_page_table->AccessPermission2 = 3;
    third_page_table->AccessPermission3 = 3;
    third_page_table->BaseAddress = (0x09000000 >> 12);

    page_directory[0x90].DescriptorType = 1;
    page_directory[0x90].ShouldBeZero = 0;
    page_directory[0x90].Domain = USER_DOMAIN;
    page_directory[0x90].ImplementationDefined = 0;
    page_directory[0x90].BaseAddress = (((uint32_t)third_page_table) >> 10);

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
    dacr.domain0 = 1;   // 内核域允许必须是由页表条目控制访问权
    dacr.domain15 = 3;  // 用户域可以不检查访问权（不知道为什么一检查就不能访问了，AP全部3也不行）
    set_dacr(&dacr);

    // 设置TTBCR寄存器，使用TTBR0和TTBR1
    TTBCR_t ttbcr;
    // 取当前TTBCR值
    get_ttbcr(&ttbcr);
    ttbcr.N = 1;
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
