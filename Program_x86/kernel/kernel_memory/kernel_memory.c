//
// Created by huangcheng on 2024/5/28.
//

#include "kernel_memory.h"

#include "../kernel_page/kernel_page.h"
#include "../kernel_task/kernel_task.h"

// 目前的内存分配设想
// 1. 0到0x1500作为内核栈使用，0x1500到0x100000是内核范围，0x9f000处是内核的TCB
// 4. 0x100000是内核页目录表，紧跟着的是1024条页表项，映射低端4MB（不然覆盖不到位图范围，没办法内存管理），还有高端的1MB（虽然只有256条页表项，但是向上取整也需要一页），共2页
// 2. 0x102000是全局物理位图起点，物理位图按照32位最大支持寻址范围4GB计算，需要128KB（32页）
// 3. 0x122000是内核虚拟位图起点，按照内核虚拟地址最高使用高端1GB计算，需要32KB（8页）

// 5. 内核预留内存8MB足够了，取个整数，内核堆内存管理范围从0x130000开始，到0x800000结束

// 因为内核的TCB也没多大用处（内核有自己单独的栈0到0x1500），干脆把全局物理位图管理结构放在0x9f800处，内核的虚拟位图管理结构就用内核的TCB里面的

// 一页大小4096字节，4KB
#define PG_SIZE 0x1000

// 全局物理地址位图结构地址
#define GLOBAL_BITMAP_PHYSICAL_ADDR 0x9f800
// 全局物理地址位图基址
#define GLOBAL_BITMAP_PHYSICAL_BASE 0x103000
// 管理4G内存时位图占据的内存最大长度（128KB）
#define GLOBAL_BITMAP_PHYSICAL_BYTE_LENGTH 0x20000

// 内核虚拟地址起点（1MB以下全部被内核使用了，无法再用做堆内存了）
#define KERNEL_VIRTUAL_ADDR_START 0xc0100000
// 内核虚拟地址位图基址
#define KERNEL_BITMAP_VIRTUAL_BASE 0x123000
// 内核只占据高端1GB的内存，所以最多只需要32KB内存就行了
// GLOBAL_BITMAP_PHYSICAL_BYTE_LENGTH / 4 = 0x8000 即可
// 不过需要注意的是，内核虚拟内存基址是0xc0100000，多了1MB
// 干脆只按照512MB来算吧，那只需要0x4000足够了
#define KERNEL_BITMAP_VIRTUAL_BYTE_LENGTH 0x4000

// 全局物理内存位图，结构在0x9f400处
BitMap *global_bitmap_physical_memory = (BitMap *)GLOBAL_BITMAP_PHYSICAL_ADDR;



// 页表、位图结束的地方，就是内核堆内存开始的地方（物理位置）
#define KERNEL_PHYSICAL_ADDR_START 0x130000
// 相应地，怕不太够用，不然连几个内核任务都开不了，所以内核保留物理内存扩大到16MB
// 16MB，有超过14MB的空余内存。一个内核任务的TCB要一页也就是4KB
// 1MB可以开 1024 / 4 = 256个内核任务
// 这么多下来足够了

// 用户堆内存开始的地方就是内核堆内存结束的地方（物理位置）
#define USER_PHYSICAL_ADDR_START 0x01000000

// 内核任务 TCB 的固定位置和栈位置
#define KERNEL_TCB ((struct task*)0x9f000)

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
    // 物理地址位图，一页是一位，一字节8位
    global_bitmap_physical_memory->bits = (uint8_t *)GLOBAL_BITMAP_PHYSICAL_BASE;
    // 这里做个临时修改，把最后的16MB留出给ramdisk，其他的就是给内存管理自由发挥
    global_bitmap_physical_memory->btmp_bytes_len = (total_physical_memory - 0x01000000) / PG_SIZE / 8;
    bitmap_init(global_bitmap_physical_memory);
    // 物理地址上已经占据了的置为1
    // 低端1MB全部被内核占据了，还有页表和页目录表、位图
    bitmap_set_range(global_bitmap_physical_memory, 0, 0x12a000 / PG_SIZE, 1);

    // 虚拟地址管理的设置（内核虚拟地址写在线程的TCB里面）
    KERNEL_TCB->process_virtual_address.virtual_addr_start = KERNEL_VIRTUAL_ADDR_START;
    // 内核虚拟位图的地址（不知道什么时候不小心删掉的，今天才发现）
    KERNEL_TCB->process_virtual_address.bitmap_virtual_memory.bits = (uint8_t *)KERNEL_BITMAP_VIRTUAL_BASE;
    // 内核虚拟地址上限
    KERNEL_TCB->process_virtual_address.bitmap_virtual_memory.btmp_bytes_len = KERNEL_BITMAP_VIRTUAL_BYTE_LENGTH;
    bitmap_init(&KERNEL_TCB->process_virtual_address.bitmap_virtual_memory);
}

// 获取一个物理页
// 参数为起始地址，结束地址，在这个范围内搜寻（这里不做太多处理了，总而言之就是不要越界）
void* alloc_physical_pages(uint32_t start_addr, uint32_t end_addr) {
    if(start_addr >= end_addr) {
        return NULL;
    }

    int32_t bit_idx = bitmap_scan_in_range(global_bitmap_physical_memory, start_addr / PG_SIZE, end_addr / PG_SIZE ,1);    // 找一个物理页面

    if (bit_idx == -1 ) {
        return NULL;
    }

    bitmap_set(global_bitmap_physical_memory, bit_idx, 1);	// 将此位bit_idx置1
    uint32_t page_phyaddr = (bit_idx * PG_SIZE);

    return (void*)page_phyaddr;
}

// 释放一个物理页
void free_physical_pages(void *page_phyaddr) {
    if(page_phyaddr == NULL) {
        return;
    }
    uint32_t bit_idx = (uint32_t)page_phyaddr / PG_SIZE;
    if (bit_idx >= global_bitmap_physical_memory->btmp_bytes_len * 8 ) {
        // 越界处理
        return;
    }
    bitmap_set(global_bitmap_physical_memory, bit_idx, 0);	// 将此位bit_idx置0
}

// 根据虚拟地址位图分配连续的虚拟页
void* alloc_virtual_pages(Virtual_Addr *virtualaddr , uint32_t pg_cnt){

    int32_t bit_idx_start = bitmap_scan(&virtualaddr->bitmap_virtual_memory, pg_cnt);  // 扫描位图，找到连续的空闲页
    if (bit_idx_start == -1) {
        return NULL;  // 没有足够的连续空闲页
    }
    bitmap_set_range(&virtualaddr->bitmap_virtual_memory, bit_idx_start, pg_cnt, 1); // 标记这些页为已使用

    uint32_t vaddr_start = bit_idx_start * PG_SIZE + virtualaddr->virtual_addr_start;  // 计算虚拟地址

    return (void*)vaddr_start;
}

// 根据虚拟地址位图释放虚拟页
void free_virtual_pages(Virtual_Addr *virtualaddr, void* vaddr, uint32_t pg_cnt){
    if(vaddr == NULL || pg_cnt == 0) {
        return;
    }
    // 更新虚拟地址位图，将虚拟页标记为未使用
    uint32_t bit_idx_start = ((uint32_t)vaddr - virtualaddr->virtual_addr_start) / PG_SIZE;
    bitmap_set_range(&virtualaddr->bitmap_virtual_memory, bit_idx_start, pg_cnt, 0);
}

// 添加映射
int8_t add_page_mapping(uint32_t virtual_address, uint32_t physical_address) {

    VaddrPart info = {.addr_value = virtual_address};
    // 只要最后一项自引用了，0xfffff000就是当前页目录表所在的地方，找页目录表地址就很方便了，直接变成了常量
    page_directory_entry_t *pde = ((page_directory_entry_t *)0xfffff000 + info.part.pde_index);
    // 检查是否还没有建立
    if(!pde->present) {
        // 还没有建立的话，要给它建立对应的页表
        // 页表项无论如何都必须放在内核空间，不管你是用户进程还是内核
        page_table_entry_t *page_table =  alloc_physical_pages(KERNEL_PHYSICAL_ADDR_START, USER_PHYSICAL_ADDR_START);
        if(page_table == NULL) {
            // 这里还没什么资源，就做了个转换而已，不用回收资源
            return 0;
        }
        // 清空内存空间
        memset(page_table, 0, PG_SIZE);

        pde->present = 1;   // 页目录表存在
        pde->us = 0;        // 超级用户权限
        pde->rw = 1;        // 可读写
        pde->table = (uint32_t)page_table >> 12;
    }

    page_table_entry_t *pte = ((page_table_entry_t *)(pde->table << 12) + info.part.pte_index);

    // 已有映射
    if(pte->present) {
        // 已有的映射才会失败，那是不需要回滚的
        // 前面申请空间，已经清空，不可能到这里
        return 0;
    }

    // 还没有映射，直接建立映射项目
    pte->present = 1;   // 页表项存在
    pte->us = 0;        // 超级用户权限
    pte->rw = 1;        // 可读写
    pte->frame = physical_address >> 12;    // 写入物理地址，建立映射

    return 1;
}

// 移除映射，返回对应的物理页表地址
void *remove_page_mapping(uint32_t virtual_address) {
    VaddrPart info = {.addr_value = virtual_address};

    // 获取页目录项
    page_directory_entry_t *pde = ((page_directory_entry_t *)0xfffff000 + info.part.pde_index);

    // 如果页目录项不存在，直接返回NULL
    if (!pde->present) {
        return NULL;
    }

    // 获取页表项
    page_table_entry_t *pte = ((page_table_entry_t *)(pde->table << 12) + info.part.pte_index);

    // 如果页表项不存在，直接返回NULL
    if (!pte->present) {
        return NULL;
    }

    // 获取物理地址
    uint32_t physical_address = pte->frame << 12;
    // 清除页表项
    memset(pte, 0, sizeof(page_table_entry_t));

    // 检查页表中是否还有有效的页表项
    page_table_entry_t *page_table = (page_table_entry_t *)(pde->table << 12);
    uint8_t page_table_empty = 1;
    for (int i = 0; i < 1024; i++) {
        if (page_table[i].present) {
            page_table_empty = 0;
            break;
        }
    }

    // 如果页表为空，释放页表并清空页目录项
    if (page_table_empty) {
        free_physical_pages(page_table);
        memset(pde, 0, sizeof(page_directory_entry_t));
    }

    return (void *)physical_address;
}

// 申请以页为单位的内存
void* malloc_page(MemoryRequesterType type, uint32_t pg_cnt) {
    uint32_t start_addr, end_addr;

    if(type == KERNEL_FLAG) {
        // 设置内核物理内存范围
        start_addr = KERNEL_PHYSICAL_ADDR_START;
        end_addr = USER_PHYSICAL_ADDR_START;
    } else {
        // 设置用户物理内存范围
        start_addr = USER_PHYSICAL_ADDR_START;
        end_addr = (uint32_t)(global_bitmap_physical_memory->btmp_bytes_len * 8 * PG_SIZE);
    }

    // 获取当前任务
    Virtual_Addr *virtual_addr = &running_task()->process_virtual_address;

    // 首先申请虚拟页
    void *vaddr = alloc_virtual_pages(virtual_addr, pg_cnt);
    if(vaddr == NULL) {
        return NULL;
    }

    // 然后再一个个申请物理页做映射
    // 因为要做到原子操作，所以申请失败必须全部回滚
    // 我这里用时间换空间，直接用数组把页数据存起来，如果执行不了就全部回滚
    // 再用一个标记记录是否失败，一旦失败全部回滚先前操作
    uint8_t success = 1;

    // 存储物理页数据的数组，清空待用
    void *pages[pg_cnt];
    for(uint32_t i = 0; i < pg_cnt; i++) {
        pages[i] = NULL;
    }

    for(uint32_t i = 0; i < pg_cnt; i++) {
        pages[i] = alloc_physical_pages(start_addr, end_addr);
        if(pages[i] == NULL) {
            success = 0;
            break;
        }
    }
    if (!success) {
        // 失败回滚
        free_virtual_pages(virtual_addr, vaddr, pg_cnt);
        for (uint32_t i = 0; i < pg_cnt; i++) {
            if (pages[i] != NULL) {
                free_physical_pages(pages[i]);
            }
        }
        return NULL;
    }
    // 逐个映射
    for (uint32_t i = 0; i < pg_cnt; i++) {
        success = add_page_mapping((uint32_t)vaddr + i * PG_SIZE, (uint32_t)pages[i]);
        if (!success) {
            // 一个失败，全部回滚
            for (uint32_t j = 0; j < i; j++) {
                remove_page_mapping((uint32_t)vaddr + j * PG_SIZE);
                free_physical_pages(pages[j]);
            }
            free_virtual_pages(virtual_addr, vaddr, pg_cnt);
            return NULL;
        }
    }

    return vaddr;
}

void free_page(void* vaddr, uint32_t pg_cnt) {
    if (vaddr == NULL || pg_cnt == 0) {
        return;
    }

    Virtual_Addr *virtual_addr = &running_task()->process_virtual_address;

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

    // 释放虚拟页
    free_virtual_pages(virtual_addr, vaddr, pg_cnt);
}
