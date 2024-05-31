//
// Created by huangcheng on 2024/5/28.
//

#include "kernel_memory.h"

#include "../kernel_page/kernel_page.h"

// 目前的内存分配设想
// 1. 0到0x900作为栈使用
// 2. 0x900（Loader入口）到0x1500（内核入口）虽然用过了，但是里面还有个内存容量数据，不能覆盖
// 3. 0x100000是页目录表起点，如果管理的内存是4GB，页目录表+页表要4KB+4MB
// 4. 内核无论如何预留最少8MB，根据内存容量来，如果内存容量大，就收多一点，但是无论再小起码8MB，不然不够用。
// 所以，0到0x7fffff给内核的内存，0x800000开始才是给用户的内存
// 0x100000到0x500fff必须标记为已使用（这4MB+4KB是页表和页目录表）
// 如果内存容量超过3GB，接触到映射低端内存的虚拟地址了。用户内存池初始化的时候要注意这一段。
// 管理4G内存的物理地址位图，至少要128KB。因此要划分出一块区域。
// 考虑到未来的扩充，暂定内核最多占据2GB内存，那么至少内核虚拟地址位图需要64KB内存，即0x10000字节。
// 0xa0000到0xfffff是硬件保留位置。不能使用。
// 干脆紧跟着页表数据
// 暂时选择0x501000到0x520fff存放物理位图，0x521000到0x530ffff存放内核虚拟位图，反正不够再挪

// 全局物理地址位图基址
#define GLOBAL_BITMAP_PHYSICAL_BASE 0x501000
// 内核虚拟地址位图基址
#define KERNEL_BITMAP_VIRTUAL_BASE 0x521000
// 管理4G内存时位图占据的内存最大长度（128KB）
#define MAX_BITMAP_BYTE_LENGTH 0x20000

// 一页大小4096字节，4KB
#define PG_SIZE 0x1000

// 页表结束的地方，就是内核堆内存开始的地方（物理位置）
#define KERNEL_PHYSICAL_ADDR_START 0x00501000
// 用户堆内存开始的地方就是内核堆内存结束的地方（物理位置）
#define USER_PHYSICAL_ADDR_START 0x00800000
// 内核虚拟地址起点（1MB以下全部被内核使用了，无法再用做堆内存了）
#define KERNEL_VIRTUAL_ADDR_START 0xc0100000


BitMap global_bitmap_physical_memory;  // 全局物理内存位图

//管理内核的虚拟地址
Virtual_Addr kernel_virtual_addr;

//  在32位系统中，虚拟地址通常分为三部分：
//  高10位：页目录索引（PDE Index）
//  中10位：页表索引（PTE Index）
//  低12位：页内偏移（Offset within the page）
typedef union {
    uint32_t addr_value;
    struct Part {
        uint32_t offset : 12;       // 低12位：页内偏移
        uint32_t pte_index : 10;    // 中10位：页表索引
        uint32_t pde_index : 10;    // 高10位：页目录索引
    }part;
}VaddrPart;

// 内存管理初始化
void init_memory(uint32_t total_physical_memory) {

    // 已经被占用的就不用管理了，比如页表、比如位图，这两个动不得
    // 物理地址位图，一页是一位，一字节8位
    global_bitmap_physical_memory.bits = (uint8_t *)GLOBAL_BITMAP_PHYSICAL_BASE;
    global_bitmap_physical_memory.btmp_bytes_len = total_physical_memory / PG_SIZE / 8;
    // 初始化物理位图
    bitmap_init(&global_bitmap_physical_memory);
    // 物理地址上已经占据了的置为1
    // 低端1MB全部被内核占据了，还有页表和页目录表
    bitmap_set_range(&global_bitmap_physical_memory, 0, (KERNEL_PHYSICAL_ADDR_START - 0) / PG_SIZE, 1);

    // 虚拟地址管理的设置
    kernel_virtual_addr.virtual_addr_start = KERNEL_VIRTUAL_ADDR_START;
    // 内核虚拟地址和物理地址一一对应，因此长度上也要和内核堆内存相同
    kernel_virtual_addr.bitmap_virtual_memory.bits = (uint8_t *)KERNEL_BITMAP_VIRTUAL_BASE;
    kernel_virtual_addr.bitmap_virtual_memory.btmp_bytes_len = (USER_PHYSICAL_ADDR_START - KERNEL_PHYSICAL_ADDR_START) / PG_SIZE / 8;

}

// 根据位图分配连续的虚拟页
void* alloc_virtual_pages(Virtual_Addr virtualaddr , uint32_t pg_cnt){

    int32_t bit_idx_start = bitmap_scan(&virtualaddr.bitmap_virtual_memory, pg_cnt);  // 扫描位图，找到连续的空闲页
    if (bit_idx_start == -1) {
        return NULL;  // 没有足够的连续空闲页
    }
    bitmap_set_range(&virtualaddr.bitmap_virtual_memory, bit_idx_start, pg_cnt, 1); // 标记这些页为已使用

    uint32_t vaddr_start = bit_idx_start * PG_SIZE + virtualaddr.virtual_addr_start;  // 计算虚拟地址

    return (void*)vaddr_start;
}

// 释放虚拟页
void free_virtual_pages(Virtual_Addr virtualaddr, void* vaddr, uint32_t pg_cnt){
    if(vaddr == NULL || pg_cnt == 0) {
        return;
    }
    // 更新虚拟地址位图，将虚拟页标记为未使用
    uint32_t bit_idx_start = ((uint32_t)vaddr - virtualaddr.virtual_addr_start) / PG_SIZE;
    bitmap_set_range(&kernel_virtual_addr.bitmap_virtual_memory, bit_idx_start, pg_cnt, 0);
}

// 获取一个物理页
void* alloc_physical_pages(MemoryRequesterType type) {
    int32_t bit_idx = -1;
    uint32_t page_phyaddr = 0;
    if(type == KERNEL_FLAG) {
        bit_idx = bitmap_scan_in_range(&global_bitmap_physical_memory, KERNEL_PHYSICAL_ADDR_START / PG_SIZE, USER_PHYSICAL_ADDR_START / PG_SIZE ,1);    // 找一个物理页面
    } else {
        bit_idx = bitmap_scan_in_range(&global_bitmap_physical_memory, USER_PHYSICAL_ADDR_START / PG_SIZE, global_bitmap_physical_memory.btmp_bytes_len * 8 ,1);    // 找一个物理页面
    }
    if (bit_idx == -1 ) {
        return NULL;
    }
    bitmap_set(&global_bitmap_physical_memory, bit_idx, 1);	// 将此位bit_idx置1
    page_phyaddr = (bit_idx * PG_SIZE);


    return (void*)page_phyaddr;
}
// 释放一个物理页
void free_physical_pages(void *page_phyaddr) {
    if(page_phyaddr == NULL) {
        return;
    }
    uint32_t bit_idx = (uint32_t)page_phyaddr / PG_SIZE;
    if (bit_idx >= global_bitmap_physical_memory.btmp_bytes_len * 8 ) {
        // 越界处理
        return;
    }
    bitmap_set(&global_bitmap_physical_memory, bit_idx, 0);	// 将此位bit_idx置0

}

// 添加映射
int8_t add_page_mapping(uint32_t virtual_address, uint32_t physical_address) {

    VaddrPart info = {.addr_value = virtual_address};
    // 所有目录项在初始化的时候都已经存在了，不用管了
    // 只是页表项地址还没有解决而已
    uint32_t *page_table = (uint32_t *)(PAGE_DIR_TABLE_POS + 0x1000 + info.part.pde_index * 0x1000 + sizeof(uint32_t) * info.part.pte_index);

    typedef union {
        uint32_t value;
        page_table_entry_t page_table_entry;
    } TableTransfer;

    TableTransfer tableTransfer = {.value = *page_table};

    if (tableTransfer.page_table_entry.present) {
        // 如果已经存在映射，可以选择返回错误或覆盖旧映射
        // 这里选择直接失败返回
        return 0;
    }

    // 设置页表项
    tableTransfer.page_table_entry.present = 1;
    tableTransfer.page_table_entry.rw = 1;
    tableTransfer.page_table_entry.us = 0;
    tableTransfer.page_table_entry.frame = physical_address >> 12;

    // 写入，建立映射
    *(page_table) = tableTransfer.value;

    return 1;
}
// 移除映射，返回对应的物理页表地址
void *remove_page_mapping(uint32_t virtual_address) {

    VaddrPart info = {.addr_value = virtual_address};
    // 所有目录项在初始化的时候都已经存在了，不用管了
    // 只是页表项地址还没有解决而已
    uint32_t *page_table = (uint32_t *)(PAGE_DIR_TABLE_POS + 0x1000 + info.part.pde_index * 0x1000 + sizeof(uint32_t) * info.part.pte_index);
    typedef union {
        uint32_t value;
        page_table_entry_t page_table_entry;
    } TableTransfer;
    TableTransfer tableTransfer = {.value = *page_table};
    uint32_t physical_address = tableTransfer.page_table_entry.frame >> 12;
    // 直接移除全部数据
    *(page_table) = 0;

    return (void *)physical_address;
}

// 申请以页为单位的内存
void* malloc_page(MemoryRequesterType type, uint32_t pg_cnt){

    if(type == KERNEL_FLAG) {
        // 首先申请虚拟页
        void *vaddr = alloc_virtual_pages(kernel_virtual_addr, pg_cnt);
        if(vaddr == NULL) {
            return NULL;
        }
        // 然后再一个个申请做映射
        // 因为要做到原子操作，所以申请失败必须全部回滚
        // 我这里用时间换空间，直接用数组把页数据存起来，如果执行不了就全部回滚
        uint8_t success = 1;
        void *pages[pg_cnt];
        for(uint32_t i = 0; i < pg_cnt; i++) {
            pages[i] = NULL;
        }
        for(uint32_t i = 0; i < pg_cnt; i++) {
            pages[i] = alloc_physical_pages(KERNEL_FLAG);
            if(pages[i] == NULL) {
                success = 0;
                break;
            }
        }
        if (!success) {
            // 失败回滚，删除虚拟页，释放物理页
            free_virtual_pages(kernel_virtual_addr, vaddr, pg_cnt);
            for (uint32_t i = 0; i < pg_cnt; i++) {
                if (pages[i] != NULL) {
                    free_physical_pages(pages[i]);
                }
            }
            return NULL;
        }
        // 到这里还没失败说明都成功了，逐个建立映射
        for (uint32_t i = 0; i < pg_cnt; i++) {

            success = add_page_mapping((uint32_t)vaddr + i * PG_SIZE, (uint32_t)pages[i]);

            if (!success) {
                // 失败回滚，删除已建立的映射，释放物理页
                for (uint32_t j = 0; j < i; j++) {
                    remove_page_mapping((uint32_t)vaddr + j * PG_SIZE);
                    free_physical_pages(pages[j]);
                }
                free_virtual_pages(kernel_virtual_addr, vaddr, pg_cnt);
                return NULL;
            }
        }

        return vaddr;

    } else {
        // 用户级实现等进程实现了再说
        return NULL;
    }

}

void free_page(MemoryRequesterType type, void* vaddr, uint32_t pg_cnt) {
    if (vaddr == NULL || pg_cnt == 0) {
        return;
    }

    if(type == KERNEL_FLAG) {
        // 逐个释放页表中的物理页
        for (uint32_t i = 0; i < pg_cnt; i++) {
            // 计算虚拟地址
            uint32_t virtual_address = (uint32_t)vaddr + i * PG_SIZE;
            // 移除映射关系
            void* page_phyaddr = remove_page_mapping(virtual_address);
            // 释放物理页
            if (page_phyaddr != NULL) {
                free_physical_pages(page_phyaddr);
            }
        }

        free_virtual_pages(kernel_virtual_addr, vaddr, pg_cnt);

    } else {
        // 等实现了用户进程再说
        return;
    }
}
