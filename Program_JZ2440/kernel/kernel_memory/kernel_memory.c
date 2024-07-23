//
// Created by huangcheng on 2024/5/28.
//

#include "kernel_memory.h"

#include "../kernel_page/kernel_page.h"
#include "../kernel_task/kernel_task.h"

// JZ2440的偏移量是0x30000000
// 基于此进行修改
// 映射的建立也要根据kernel_page.h中的页表结构进行修改

// 一页大小4096字节，4KB
#define PG_SIZE 0x1000

// 全局物理地址位图结构地址
#define GLOBAL_BITMAP_PHYSICAL_ADDR 0x3009f800
// 全局物理地址位图基址（因为arm的一级页表占0x4000，现在又没有二级页表）
#define GLOBAL_BITMAP_PHYSICAL_BASE 0x30104000
// 管理4G内存时位图占据的内存最大长度（128KB）
#define GLOBAL_BITMAP_PHYSICAL_BYTE_LENGTH 0x20000

// 内核虚拟地址起点（注意，和x86一样，1MB以下全部被内核使用了，无法再用做堆内存了）
#define KERNEL_VIRTUAL_ADDR_START 0xc0000000
// 内核虚拟地址位图基址
#define KERNEL_BITMAP_VIRTUAL_BASE 0x30124000
// 内核只占据高端1GB的内存，所以最多只需要32KB内存就行了
// GLOBAL_BITMAP_PHYSICAL_BYTE_LENGTH / 4 = 0x8000 即可
// 之前在x86中，因为多了1MB的问题，就把内核虚拟地址大小缩减到了512MB
// 可以直接设置为已经占据，这样起算就方便了
#define KERNEL_BITMAP_VIRTUAL_BYTE_LENGTH 0x4000

// 全局物理内存位图，结构在0x9f800处
BitMap *global_bitmap_physical_memory = (BitMap *)GLOBAL_BITMAP_PHYSICAL_ADDR;

// 页表、位图结束的地方，就是内核堆内存开始的地方（物理位置，放在0x130000这里开始是为了取整，这里不加上偏移量是为了位图计算）
#define KERNEL_PHYSICAL_ADDR_START 0x130000
// 相应地，怕不太够用，不然连几个内核任务都开不了，所以内核保留物理内存扩大到16MB
// 16MB，有超过14MB的空余内存。一个内核任务的TCB要一页也就是4KB
// 1MB可以开 1024 / 4 = 256个内核任务
// 这么多下来足够了

// 用户堆内存开始的地方就是内核堆内存结束的地方（物理位置，这里不加上偏移量是为了位图计算）
#define USER_PHYSICAL_ADDR_START 0x1000000

// 内核任务 TCB 的固定位置和栈位置
#define KERNEL_TCB ((struct task*)0x3009f000)

// 32位x86的虚拟地址拆分方式不适合32位arm
// 32位arm的虚拟地址拆分方式如下：
typedef union {
    uint32_t addr_value;
    struct Part {
        uint32_t offset : 12;       // 低12位：页内偏移
        uint32_t pte_index : 8;     // 中8位：页表索引，其实就是二级页表索引，这里我只不过是懒得换名字了
        uint32_t pde_index : 12;    // 高12位：页目录索引，其实就是一级页表索引，这里我只不过是懒得换名字了
    }part;
}VaddrPart;

// 内存管理初始化
void init_memory(uint32_t total_physical_memory) {
    // 物理地址位图，一页是一位，一字节8位
    global_bitmap_physical_memory->bits = (uint8_t *)GLOBAL_BITMAP_PHYSICAL_BASE;
    global_bitmap_physical_memory->btmp_bytes_len = total_physical_memory / PG_SIZE / 8;
    bitmap_init(global_bitmap_physical_memory);
    // 物理地址上已经占据了的置为1
    // 低端1MB全部被内核占据了，还有页表、位图（按照上面取整，直接按0x130000计算，以下全部当作被占据）
    bitmap_set_range(global_bitmap_physical_memory, 0, 0x130000 / PG_SIZE, 1);

    // 虚拟地址管理的设置（内核虚拟地址写在线程的TCB里面）
    KERNEL_TCB->process_virtual_address.virtual_addr_start = KERNEL_VIRTUAL_ADDR_START;
    // 内核虚拟地址的地址
    KERNEL_TCB->process_virtual_address.bitmap_virtual_memory.bits = (uint8_t *)KERNEL_BITMAP_VIRTUAL_BASE;
    // 内核虚拟地址上限
    KERNEL_TCB->process_virtual_address.bitmap_virtual_memory.btmp_bytes_len = KERNEL_BITMAP_VIRTUAL_BYTE_LENGTH;
    // 初始化内核虚拟位图
    bitmap_init(&(KERNEL_TCB->process_virtual_address.bitmap_virtual_memory));
    // 设置最开始的1MB全部被内核占据了
    bitmap_set_range(&(KERNEL_TCB->process_virtual_address.bitmap_virtual_memory), 0, 0x100000 / PG_SIZE, 1);
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
    // 这里要加上DRAM偏移量
    uint32_t page_phyaddr = (bit_idx * PG_SIZE + DRAM_OFFSET);

    return (void*)page_phyaddr;
}

// 释放一个物理页
void free_physical_pages(void *page_phyaddr) {
    if(page_phyaddr == NULL) {
        return;
    }
    // 这里要减去DRAM偏移量
    uint32_t bit_idx = ((uint32_t)page_phyaddr - DRAM_OFFSET) / PG_SIZE;
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
    // 因为用的是段描述符，所以自引用地址变成0xfff00000
    // 要按照4KB粒度内存分配的话，只能按照一级粗页表和二级小页表的方式来做
    CoarsePageTableEntry *pde = ((CoarsePageTableEntry *)0xfff00000 + info.part.pde_index);
    // 检查是否还没有建立
    if(pde->DescriptorType == 0 || pde->BaseAddress == 0) {
        // 首先，解释一下逻辑
        // 尽可能像x86一样做，不要浪费页表项
        // 但是arm的一级页表是4096条，二级页表是256条
        // 所以为了不浪费，如果这条没有，就要以连续四条页表同时指定
        // 因为先前没有
        // 进入这个逻辑的时候，如果pde_index是0，那么就连续指定0、1、2、3
        // 下次进入这个逻辑的时候，pde_index就是4
        // 以此类推

        // 还没有建立的话，要给它建立对应的页表
        // 页表项无论如何都必须放在内核空间，不管你是用户进程还是内核
        SmallPageDescriptor *page_table =  alloc_physical_pages(KERNEL_PHYSICAL_ADDR_START, USER_PHYSICAL_ADDR_START);
        if(page_table == NULL) {
            // 这里还没什么资源，就做了个转换而已，不用回收资源
            return 0;
        }
        // 清空内存空间
        memset(page_table, 0, PG_SIZE);

        // 连续设置4条粗页表
        for(uint32_t i = 0; i < 4; i++) {
            pde[i].DescriptorType = 1;
            pde[i].ShouldBeZero = 0;
            pde[i].ShouldBeOne = 1;
            // 域控制器设置了两个域都是全权，也没区别了
            // 如果要精细控制的话，需要加参数确定当前是什么角色，指定什么域
            pde[i].Domain = USER_DOMAIN;
            pde[i].ShouldBeZero2 = 1;
            pde[i].BaseAddress = (((uint32_t)(page_table) + i * 0x400) >> 10);
        }
    }

    SmallPageDescriptor *pte = ((SmallPageDescriptor *)(pde->BaseAddress << 10) + info.part.pte_index);

    // 任何一个不为0说明已有映射
    if(pte->PageType || pte->BaseAddress) {
        // 已有的映射才会失败，那是不需要回滚的
        // 前面申请空间，已经清空，不可能到这里
        return 0;
    }

    // 还没有映射，直接建立映射项目
    pte->PageType = 2;
    pte->Bufferable = 0;
    pte->Cacheable = 0;
    pte->AccessPermission0 = 3;
    pte->AccessPermission1 = 3;
    pte->AccessPermission2 = 3;
    pte->AccessPermission3 = 3;
    pte->BaseAddress = physical_address >> 12;  // 写入物理地址，建立映射

    return 1;
}

// 移除映射，返回对应的物理页表地址
void *remove_page_mapping(uint32_t virtual_address) {
    VaddrPart info = {.addr_value = virtual_address};

    // 因为用的是段描述符，所以自引用地址变成0xfff00000
    // 获取页目录项
    CoarsePageTableEntry *pde = ((CoarsePageTableEntry *)0xfff00000 + info.part.pde_index);

    // 如果页目录项不存在，直接返回NULL
    if (pde->DescriptorType == 0 || pde->BaseAddress == 0) {
        return NULL;
    }

    // 获取页表项
    SmallPageDescriptor *pte = ((SmallPageDescriptor *)(pde->BaseAddress << 10) + info.part.pte_index);

    // 如果页表项不存在，直接返回NULL
    if (pte->PageType == 0 || pte->BaseAddress == 0) {
        return NULL;
    }

    // 获取物理地址
    uint32_t physical_address = pte->BaseAddress << 12;
    // 清除页表项
    memset(pte, 0, sizeof(SmallPageDescriptor));

    // 注意：要回收的话，意味着一次性要回收四个粗页表项
    // 因此需要额外计算

    // 找到这四个粗页表项的开头，并取出它的页表项
    // 用整数除法，移除小数只留下整数结果，再乘以4就能得到这组粗页表项的开头索引了
    CoarsePageTableEntry * page_directory = (CoarsePageTableEntry *)0xfff00000 + ((info.part.pde_index / 4) * 4);
    SmallPageDescriptor *page_table = (SmallPageDescriptor *)(page_directory->BaseAddress << 10);

    // 检查页表中是否还有有效的页表项
    uint8_t page_table_empty = 1;
    for (int i = 0; i < 1024; i++) {
        if (page_table[i].PageType || pte->BaseAddress) {
            // 任何一个有都说明有效
            page_table_empty = 0;
            break;
        }
    }

    // 如果页表为空，释放整张页表并清空连续四个页目录项
    if (page_table_empty) {
        free_physical_pages(page_table);
        memset(page_directory, 0, 4 * sizeof(CoarsePageTableEntry));
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
