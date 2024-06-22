//
// Created by huangcheng on 2024/6/20.
//

#include "fs_int.h"
#include "../kernel/kernel_memory/kernel_memory.h"

// 超级块是否有效
bool is_valid_superblock() {

    // 读入第0扇区的数据到fs_buffer
    read_sector(0);
    SuperBlock * block = (SuperBlock *)fs_buffer;
    if(strcmp(block->magic_header, "HCFileSystemMag") != 0) {
        return false;
    }
    if(strcmp(block->magic_footer, "SuperBlockValid") != 0) {
        return false;
    }
    if(block->root_dir_sector != 0) {
        return false;
    }
    // 这个纯粹校验计算准不准
    if(block->bitmap_sectors != block->total_sectors >> 12) {
        return false;
    }
    // 校验通过
    return true;
}

// 初始化超级块，参数是外存一共有多少个扇区
void init_superblock(uint32_t total_sectors) {

    SBlk->total_sectors = total_sectors;
    // 有多少个扇区，计算我要用多少个扇区来作为位图
    // 一个位表示一个扇区，一个字节8个位
    // 一个扇区512字节，定死
    // SBlk->bitmap_sectors = SBlk->total_sectors / 8 / 512

    // 这个数字正好是一页4KB，直接右移操作也可以
    SBlk->bitmap_sectors = SBlk->total_sectors >> 12;

    // 在内存中建立位图
    SBlk->btmp.bits = (uint8_t*)malloc_page(KERNEL_FLAG, SBlk->bitmap_sectors / 8);
    if (SBlk->btmp.bits == NULL) {
        // 处理内存分配失败的情况
        // 恢复原样，直接返回
        SBlk->total_sectors = 0;
        SBlk->bitmap_sectors = 0;
        return;
    }
    // 位图长度计算，一个扇区512字节，多少个扇区乘以512就是总长度
    SBlk->btmp.btmp_bytes_len = SBlk->bitmap_sectors * 512;
    // 位图初始化
    bitmap_init(&SBlk->btmp);

    // 写超级块各项
    strcpy(SBlk->magic_header, "HCFileSystemMag");
    strcpy(SBlk->magic_footer, "SuperBlockValid");

    // 根目录的问题不归初始化超级块管

    // 目前约定前2MB存放超级块、位图、目录信息，文件元信息从2MB开始
    // 2MB / 512字节 = 4096
    // 扇区号从0开始计算，0是超级块，1开始是位图数量，然后到目录信息所在
    // 所以正好是4096号扇区开始（这是第4097个扇区）
    SBlk->file_meta_start_sector =  4096;
    // 现在没有任何目录信息和文件元信息，除了超级块和位图，剩下的全都是自由扇区
    // 超级块本身占一个
    SBlk->free_sectors = SBlk->total_sectors - SBlk->bitmap_sectors - 1;

    // 位图设置已经被占用的部分
    // 超级块和位图区都被占据，从0开始，一共是1 + SBlk->bitmap_sectors个位置
    bitmap_set_range(&SBlk->btmp, 0,1 + SBlk->bitmap_sectors,1);
}

// 从外存读入超级块，并恢复管理位图
void read_superblock() {
    // 读入第0扇区的数据到fs_buffer
    read_sector(0);

    // 将读入的数据复制到长期驻留内存的超级块中
    memcpy(SBlk, fs_buffer, sizeof(SuperBlock));

    // 一个扇区定死512字节，一页4KB，位图占据了多少个扇区 / 8就是所需的页数了
    SBlk->btmp.bits = (uint8_t*)malloc_page(KERNEL_FLAG, SBlk->bitmap_sectors / 8);
    if (SBlk->btmp.bits == NULL) {
        // 处理内存分配失败的情况
        return;
    }
    // 位图长度计算，一个扇区512字节，多少个扇区乘以512就是总长度
    SBlk->btmp.btmp_bytes_len = SBlk->bitmap_sectors * 512;

    // 读入位图数据
    for (uint32_t i = 0; i < SBlk->bitmap_sectors; i++) {
        read_sector(i + 1); // 位图从第1扇区开始
        // 逐个扇区读入，然后复制到位图区域中
        memcpy(SBlk->btmp.bits + i * 512, fs_buffer, 512);
    }
}

// 把超级块和管理信息写入到外存
void write_superblock() {
    // 将超级块的数据复制到fs_buffer中
    memcpy(fs_buffer, SBlk, sizeof(SuperBlock));

    // 写入第0扇区
    write_sector(0);

    // 写入位图数据
    for (uint32_t i = 0; i < SBlk->bitmap_sectors; i++) {
        // 把位图数据逐扇区复制过来
        memcpy(fs_buffer, SBlk->btmp.bits + i * 512, 512);
        // 位图从第1扇区开始
        write_sector(i + 1);
    }
}

// -------------------------   实现目录操作和文件操作所需的工具函数   -------------------------

// 计算校验和
uint32_t calculate_checksum(DirectoryInfo *dir_info) {
    uint32_t checksum = 0;
    uint8_t* data = (uint8_t*)dir_info;
    for (uint32_t i = 0; i < sizeof(DirectoryInfo) - sizeof(uint32_t) - sizeof(dir_info->fill); i++) {
        checksum += data[i];
    }
    return checksum;
}

// 更新校验和
void update_checksum(DirectoryInfo* dir_info) {
    dir_info->checksum = calculate_checksum(dir_info);
}

// 该扇区是否被使用
bool is_used_sector(uint32_t sector_idx) {
    // 我怕编译器报错
    int32_t res = bitmap_is_bit_set(&SBlk->btmp, sector_idx);
    if(res == 0) {
        return false;
    }
    if(res == 1) {
        return true;
    }
    return 1;
}

// 获取一个目录扇区（目录都是一个一个建立的，没必要连续获取）
uint32_t acquire_directory_sector() {
    // 因为扇区号从0开始
    // 0是超级块了，连续SBlk->bitmap_sectors个扇区都是位图的范围
    // 所以从SBlk->bitmap_sectors + 1开始
    // 结束地址是文件元信息的起始扇区
    int32_t new_dir_sector_idx = bitmap_scan_in_range(&SBlk->btmp, SBlk->bitmap_sectors + 1, SBlk->file_meta_start_sector, 1);

    // 没空余扇区放目录了，返回0
    if (new_dir_sector_idx == -1) {
        return 0;
    }

    // 设置该位已经使用
    bitmap_set(&SBlk->btmp, new_dir_sector_idx, 1);
    // 空闲扇区-1
    SBlk->free_sectors--;

    return new_dir_sector_idx;
}

// 释放一个目录扇区（直接把位图设置为0就得了，没必要写回外存）
void release_directory_sector(uint32_t sector_idx) {
    // 设置扇区不被使用
    bitmap_set(&SBlk->btmp, sector_idx, 0);
    // 空闲扇区+1
    SBlk->free_sectors++;
}

// 获取指定数量的文件扇区，返回起始扇区号
uint32_t acquire_file_sector(uint32_t cnt) {
    int32_t new_file_sector_idx = bitmap_scan_in_range(&SBlk->btmp, SBlk->file_meta_start_sector, SBlk->btmp.btmp_bytes_len * 8, cnt);

    // 没空余扇区放足够文件了，返回0
    if (new_file_sector_idx == -1) {
        return 0;
    }

    // 连续设置这些扇区已经被使用
    bitmap_set_range(&SBlk->btmp, new_file_sector_idx, cnt,1);
    // 空闲扇区减去cnt
    SBlk->free_sectors -= cnt;

    return new_file_sector_idx;
}

// 释放从起始地址开始的连续数量文件扇区
void release_file_sector(uint32_t sector_idx, uint32_t cnt) {
    // 连续设置这些扇区不被使用
    bitmap_set_range(&SBlk->btmp, sector_idx, cnt,1);
    // 空闲扇区加上cnt
    SBlk->free_sectors += cnt;
}


// -------------------------   实现目录操作和文件操作所需的工具函数   -------------------------

// 判断目录是否有效
bool is_valid_directory(uint32_t sector_idx) {
    // 判断是否在范围内
    // 有效的目录扇区索引从SBlk->bitmap_sectors + 1开始
    if((sector_idx < SBlk->bitmap_sectors + 1) || (sector_idx > SBlk->file_meta_start_sector)) {
        // 不在范围内，无效
        return false;
    }
    // 扇区没有被使用，无效
    if(!is_used_sector(sector_idx)) {
        return false;
    }
    read_sector(sector_idx);
    DirectoryInfo *dir_info = (DirectoryInfo*)fs_buffer;
    if (strcmp(dir_info->magic_header, "HCFileSystemDir") == 0 &&
        strcmp(dir_info->magic_header2, "ThisDirIsValid_") == 0 &&
        dir_info->sector_idx == sector_idx &&
        dir_info->checksum == calculate_checksum(dir_info)) {
        // 四项全部通过就有效
        return true;
    }
    return false;
}

// 查找目录（查找起点目录包括所有子目录，因此要递归查找）
uint32_t find_directory(const char* name, uint32_t sector_idx) {
    if (!is_valid_directory(sector_idx)) {
        // 都不是一个有效的目录，直接返回0
        return 0;
    }

    read_sector(sector_idx);
    DirectoryInfo *dir_info = (DirectoryInfo*)fs_buffer;

    // 检查当前目录是否匹配（起点目录本身，因为父目录保存的子目录索引是有信息的）
    if (strcmp(dir_info->directoryName, name) == 0) {
        return sector_idx;
    }

    // 父目录所保存的子目录结点
    // 如果起点目录不匹配，就开始往下递归找子目录
    uint32_t current_sector_idx = dir_info->subdirectory_sector_idx;

    // 如果已经到达文件树最深处，下面已经没有子目录了
    // 那么subdirectory_count必定是0，无法进入这个循环，也就无法递归
    // 也就有了递归结束条件

    // 父目录保存了子目录的数量，可以通过这个写一个for循环
    // 防止while循环造成的无限循环问题（因为每个新目录都设了自环）
    uint32_t subdirectory_count = dir_info->subdirectory_count;

    for(uint32_t i = 0; i < subdirectory_count; i++) {
        // 递归查找，深度优先
        uint32_t found_idx = find_directory(name, current_sector_idx);
        if (found_idx != 0) {
            return found_idx;
        }
        // 更换结点
        read_sector(current_sector_idx);
        // 取后兄弟
        // dir_info是个指针，重读数据之后，值也会跟着变动
        current_sector_idx = dir_info->nextSiblingDirectory_sector_idx;
    }

    // 返回0就意味着没成功，因为扇区0是超级块，很容易判断出来
    return 0;
}

// 创建根目录，只能使用一次（在初始化文件系统的时候使用）
bool create_root_directory() {
    // 申请一个新扇区
    uint32_t new_dir_sector_idx = acquire_directory_sector();

    // 没空余扇区放目录了，返回创建失败
    if (new_dir_sector_idx == 0) {
        return false;
    }

    // 全新的512字节的目录信息
    DirectoryInfo new_dir;
    // 先清空
    memset(&new_dir, 0, 512);
    // 设置各项信息
    strcpy(new_dir.magic_header, "HCFileSystemDir");
    // root的名字固定
    strcpy(new_dir.directoryName, "root");
    // 记录自己在哪个扇区，方便操作
    new_dir.sector_idx = new_dir_sector_idx;
    // 根目录不能有父目录，也不能有文件元信息
    new_dir.parentDirectory_sector_idx = 0;
    new_dir.fileMetadata_sector_idx = 0;
    strcpy(new_dir.magic_header2, "ThisDirIsValid_");

    // 因为视为双向链表的结点
    // 先把兄弟目录扇区号设置为自己，形成自环
    // 根目录不可能有兄弟，以后就是个自环
    new_dir.prevSiblingDirectory_sector_idx = new_dir_sector_idx;
    new_dir.nextSiblingDirectory_sector_idx = new_dir_sector_idx;

    // 子目录也是0，新目录哪来的子目录
    new_dir.subdirectory_count = 0;
    new_dir.subdirectory_sector_idx = 0;

    // 更新创建目录的校验和，写出这个新目录到新分配的扇区
    update_checksum(&new_dir);
    memcpy(fs_buffer, &new_dir, sizeof(DirectoryInfo));
    write_sector(new_dir.sector_idx);

    // 更新超级块这一属性
    SBlk->root_dir_sector = new_dir_sector_idx;

    return true;
}

// 创建目录（如果是一个文件的话，file_metadata_sector_idx不得为0），返回操作是否成功
bool create_directory(const char *name, uint32_t parent_sector_idx, uint32_t file_metadata_sector_idx) {
    // 没有父目录必须return，创建root目录有专门的函数
    if(parent_sector_idx == 0) {
        return false;
    }
    // file_metadata_sector_idx不是0必须校验
    if (file_metadata_sector_idx > 0 && is_valid_fileMetaInfo(file_metadata_sector_idx)) {
        return false;
    }

    // 申请一个新扇区
    uint32_t new_dir_sector_idx = acquire_directory_sector();

    // 没空余扇区放目录了，返回创建失败
    if (new_dir_sector_idx == 0) {
        return false;
    }

    // 全新的512字节的目录信息
    DirectoryInfo new_dir;
    // 先清空
    memset(&new_dir, 0, sizeof(DirectoryInfo));
    // 设置各项信息
    strcpy(new_dir.magic_header, "HCFileSystemDir");
    strcpy(new_dir.directoryName, name);
    // 记录自己在哪个扇区，方便操作
    new_dir.sector_idx = new_dir_sector_idx;
    new_dir.parentDirectory_sector_idx = parent_sector_idx;
    new_dir.fileMetadata_sector_idx = file_metadata_sector_idx;
    strcpy(new_dir.magic_header2, "ThisDirIsValid_");

    // 因为视为双向链表的结点
    // 先把兄弟目录扇区号设置为自己
    new_dir.prevSiblingDirectory_sector_idx = new_dir_sector_idx;
    new_dir.nextSiblingDirectory_sector_idx = new_dir_sector_idx;

    // 子目录也是0，新目录哪来的子目录
    new_dir.subdirectory_count = 0;
    new_dir.subdirectory_sector_idx = 0;

    //接下来的操作
    // 1. 更新父目录的子目录计数器
    // 2. 如果父目录没有子目录（子目录扇区索引为0），则将这个目录作为父目录的子目录扇区索引
    // 3. 如果父目录有子目录索引，则将父目录保存的子目录索引视为双向链表的头结点（只不过这个头结点还保存信息，注意！！），把当前结点插入到这个链表中！
    // 4. 因此还要更新父目录保存的那个作为头结点的子目录的值

    // 更新父目录信息
    // 把父目录信息从缓冲区保存到临时变量，等下要用到
    read_sector(parent_sector_idx);
    DirectoryInfo parent_dir;
    memcpy(&parent_dir, fs_buffer, 512);

    if (parent_dir.subdirectory_sector_idx == 0) {
        // 父目录没有子目录，直接将新目录设置为父目录保存的子目录结点
        parent_dir.subdirectory_sector_idx = new_dir_sector_idx;
    } else {
        // 父目录有子目录，需要使用子目录结点的prevSiblingDirectory_sector_idx找到链表尾部
        // 父目录的子目录结点是当前目录的兄弟，作为头结点存在
        read_sector(parent_dir.subdirectory_sector_idx);
        DirectoryInfo sibling_dir;
        memcpy(&sibling_dir, fs_buffer, 512);

        // 为了节省栈内存，sibling_dir要前后代表两个不同的值
        // 其实如果是只有单独一个兄弟的情况可以特殊处理，少读一个tail_sibling_dir，这里懒，为了统一，直接全读了

        // 之前已经固定每个新目录的前后兄弟目录都指向自己
        // 然后就是双向链表标准流程了
        sibling_dir.prevSiblingDirectory_sector_idx = new_dir.sector_idx;       // 头结点的prev指向新加入的结点
        new_dir.nextSiblingDirectory_sector_idx = sibling_dir.sector_idx;       // 新结点的next指向头结点

        // 更新头结点校验和，写出到外存
        update_checksum(&sibling_dir);
        memcpy(fs_buffer, &sibling_dir, 512);
        write_sector(sibling_dir.sector_idx);

        // 读最后一个结点（尾结点），此时内存中sibling_dir还是头结点的值
        read_sector(sibling_dir.prevSiblingDirectory_sector_idx);
        memcpy(&sibling_dir, fs_buffer, 512);
        // 此时sibling_dir是尾结点tail
        sibling_dir.nextSiblingDirectory_sector_idx = new_dir.sector_idx;       // 原先tail的next指向新结点
        new_dir.prevSiblingDirectory_sector_idx = sibling_dir.sector_idx;       // 新结点的prev指向原先的tail

        // 更新尾结点校验和，写出到外存
        update_checksum(&sibling_dir);
        memcpy(fs_buffer, &sibling_dir, 512);
        write_sector(sibling_dir.sector_idx);
    }

    // 更新父目录的子目录计数器
    parent_dir.subdirectory_count++;
    // 更新父目录校验和
    update_checksum(&parent_dir);
    // 写出父目录
    memcpy(fs_buffer, &parent_dir, 512);
    write_sector(parent_dir.sector_idx);


    // 更新创建目录的校验和，写出这个新目录到新分配的扇区
    update_checksum(&new_dir);
    memcpy(fs_buffer, &new_dir, 512);
    write_sector(new_dir.sector_idx);

    // 更新文件元信息的计数器
    fileMetaInfo_directoryCounter_Increment(file_metadata_sector_idx);

    return true;
}

// 移动该目录到目标目录下，成为目标目录的子目录，目标目录是新目录的父目录，返回操作是否成功
bool move_directory(uint32_t src_sector_idx, uint32_t dest_sector_idx) {
    // 目录是否为root，root不得移动
    if (src_sector_idx == SBlk->root_dir_sector) {
        return false;
    }
    // 目录是否有效
    if (!is_valid_directory(src_sector_idx)) {
        return false;
    }
    // 验证目标目录是否有效
    if (!is_valid_directory(dest_sector_idx)) {
        return false;
    }
    // 读取信息
    read_sector(src_sector_idx);
    DirectoryInfo src_dir_info;
    memcpy(&src_dir_info, fs_buffer, 512);
    // 移动位置就是当前位置（目标目录就是当前目录的父目录）
    if(src_dir_info.parentDirectory_sector_idx == dest_sector_idx) {
        // 不用移动，直接完成
        return true;
    }
    // 首先明确，移动这个目录，连带移动子目录，但是不关这个被操作目录的事情，子目录之间的关系不会被破坏
    // 破坏的是当前目录和其父目录、兄弟目录之间的关系
    // 1. 当前目录是父目录唯一的子目录：从父目录删除
    // 2. 当前目录是父目录的子目录链表头结点，而且自己还有兄弟：从兄弟链表里面删除，让自己的next兄弟取代自己的位置
    // 3. 当前目录是父目录的子目录，但不是头结点

    // 先获取父目录
    read_sector(src_dir_info.parentDirectory_sector_idx);
    DirectoryInfo parent_dir_info;
    memcpy(&parent_dir_info, fs_buffer, 512);

    // 备用，可能用来存放兄弟目录的地方
    DirectoryInfo sibling_dir_info;


    // 判断是否是唯一的子目录
    if(src_dir_info.prevSiblingDirectory_sector_idx == src_dir_info.sector_idx &&
       src_dir_info.nextSiblingDirectory_sector_idx == src_dir_info.sector_idx) {
        // 情况1
        // 直接设置父目录的子目录为0就行了
        parent_dir_info.subdirectory_sector_idx = 0;

    } else {

        // 情况2和3统一的操作

        // 取前兄弟结点，它的next直接接当前目录的next
        read_sector(src_dir_info.prevSiblingDirectory_sector_idx);
        memcpy(&sibling_dir_info, fs_buffer, 512);
        sibling_dir_info.nextSiblingDirectory_sector_idx = src_dir_info.nextSiblingDirectory_sector_idx;
        // 写出前兄弟结点到外存
        update_checksum(&sibling_dir_info);
        memcpy(fs_buffer, &sibling_dir_info, 512);
        write_sector(sibling_dir_info.sector_idx);

        // 取后兄弟结点，它的prev直接接当前目录的prev
        read_sector(src_dir_info.nextSiblingDirectory_sector_idx);
        memcpy(&sibling_dir_info, fs_buffer, 512);
        sibling_dir_info.prevSiblingDirectory_sector_idx = src_dir_info.prevSiblingDirectory_sector_idx;
        // 写出后兄弟结点到外存
        update_checksum(&sibling_dir_info);
        memcpy(fs_buffer, &sibling_dir_info, 512);
        write_sector(sibling_dir_info.sector_idx);

        if(parent_dir_info.subdirectory_sector_idx == src_dir_info.sector_idx) {
            // 情况2的特殊操作，更换头结点
            // 让后兄弟取代这个位置
            parent_dir_info.subdirectory_sector_idx = src_dir_info.nextSiblingDirectory_sector_idx;
        }
    }

    // 以防万一，先把src给回环了
    src_dir_info.nextSiblingDirectory_sector_idx = src_dir_info.sector_idx;
    src_dir_info.prevSiblingDirectory_sector_idx = src_dir_info.sector_idx;

    // 无论如何：原来父目录的计数器-1，当前目录插入到新的父目录的子目录链表里面，设置当前目录的新父目录
    parent_dir_info.subdirectory_count--;
    // 写出原父目录
    update_checksum(&parent_dir_info);
    memcpy(fs_buffer, &parent_dir_info, 512);
    write_sector(parent_dir_info.sector_idx);


    // 获取新的父目录
    read_sector(dest_sector_idx);
    memcpy(fs_buffer, &parent_dir_info, 512);
    // 从现在开始parent_dir_info是新的父目录

    src_dir_info.parentDirectory_sector_idx = parent_dir_info.sector_idx;

    // 然后就是插入新父目录的惯例操作
    // 参考create函数
    if (parent_dir_info.subdirectory_sector_idx == 0) {
        // 父目录没有子目录，直接将新目录设置为父目录保存的子目录结点
        parent_dir_info.subdirectory_sector_idx = src_dir_info.sector_idx;
    } else {
        // 父目录有子目录，需要使用子目录结点的prevSiblingDirectory_sector_idx找到链表尾部
        // 父目录的子目录结点是当前目录的兄弟，作为头结点存在

        // 这里需要记录下头结点
        read_sector(parent_dir_info.subdirectory_sector_idx);
        memcpy(&sibling_dir_info, fs_buffer, 512);

        // 然后就是双向链表标准流程了（头结点）
        sibling_dir_info.prevSiblingDirectory_sector_idx = src_dir_info.sector_idx;       // 头结点的prev指向新加入的结点
        src_dir_info.nextSiblingDirectory_sector_idx = sibling_dir_info.sector_idx;       // 新结点的next指向头结点

        // 更新头结点校验和，写出到外存
        update_checksum(&sibling_dir_info);
        memcpy(fs_buffer, &sibling_dir_info, 512);
        write_sector(sibling_dir_info.sector_idx);

        // 读最后一个结点（尾结点），此时内存中sibling_dir_info还是头结点的值
        read_sector(sibling_dir_info.prevSiblingDirectory_sector_idx);
        memcpy(&sibling_dir_info, fs_buffer, 512);
        // 此时sibling_dir_info是尾结点tail
        sibling_dir_info.nextSiblingDirectory_sector_idx = src_dir_info.sector_idx;       // 原先tail的next指向新结点
        src_dir_info.prevSiblingDirectory_sector_idx = sibling_dir_info.sector_idx;       // 新结点的prev指向原先的tail

        // 更新尾结点校验和，写出到外存
        update_checksum(&src_dir_info);
        memcpy(fs_buffer, &src_dir_info, 512);
        write_sector(src_dir_info.sector_idx);
    }

    // 更新父目录的子目录计数器
    parent_dir_info.subdirectory_count++;
    // 更新父目录校验和
    update_checksum(&parent_dir_info);
    // 写出父目录
    memcpy(fs_buffer, &parent_dir_info, 512);
    write_sector(parent_dir_info.sector_idx);

    // 更新当前目录的校验和，写出这个新目录到新分配的扇区
    update_checksum(&src_dir_info);
    memcpy(fs_buffer, &src_dir_info, 512);
    write_sector(src_dir_info.sector_idx);

    return true;
}

// 重命名目录，返回操作是否成功
bool rename_directory(uint32_t sector_idx, const char* new_name) {
    // 目录是否有效
    if (!is_valid_directory(sector_idx)) {
        return false;
    }
    // 目录有效可以操作
    read_sector(sector_idx);
    DirectoryInfo *dir_info = (DirectoryInfo*)fs_buffer;
    // 复制文件名
    strcpy(dir_info->directoryName, new_name);
    // 更新校验和
    update_checksum(dir_info);
    // 此时缓冲区中的数据就是目录数据，直接写回即可
    write_sector(sector_idx);

    return true;
}

// 递归删除，不操作外存，只让这些目录其失效即可
bool delete_directory_recursive(uint32_t sector_idx) {
    // root目录不得删除
    if (sector_idx == SBlk->root_dir_sector) {
        return false;
    }
    if (!is_valid_directory(sector_idx)) {
        // 如果目录无效，不操作
        return false;
    }
    read_sector(sector_idx);
    // 记录文件元信息参数，后面好扣减
    uint32_t file_metadata_sector_idx = ((DirectoryInfo*)fs_buffer)->fileMetadata_sector_idx;

    // 重新读入
    read_sector(sector_idx);
    DirectoryInfo *dir_info = (DirectoryInfo*)fs_buffer;

    // 深度优先，递归逻辑看查找函数
    uint32_t current_sector_idx = dir_info->subdirectory_sector_idx;
    uint32_t subdirectory_count = dir_info->subdirectory_count;

    for(uint32_t i = 0; i < subdirectory_count; i++) {
        // 递归删除，深度优先
        if( !delete_directory_recursive(current_sector_idx) ){
            // 删除失败直接退出
            return false;
        }
        // 更换结点
        read_sector(current_sector_idx);
        // 取后兄弟
        // dir_info是个指针，重读数据之后，值也会跟着变动
        current_sector_idx = dir_info->nextSiblingDirectory_sector_idx;
    }

    // 文件元信息中的目录计数器更新
    fileMetaInfo_directoryCounter_Decrement(file_metadata_sector_idx);
    // 最后release当前目录
    release_directory_sector(sector_idx);
    return true;
}

// 删除这个目录（连带子目录），返回操作是否成功
bool delete_directory(uint32_t sector_idx) {
    // root目录不得删除
    if (sector_idx == SBlk->root_dir_sector) {
        return false;
    }
    if (!is_valid_directory(sector_idx)) {
        // 如果目录无效，不操作
        return false;
    }

    // 先递归删除，如果失败则删除失败，文件系统估计要大乱
    if( !delete_directory_recursive(sector_idx) ){
        return false;
    }

    // 而后再把自己从父目录的子目录链表里面删除
    // 参考move的三种情况
    // 当前目录的前兄弟
    uint32_t prevSiblingDirectory_sector_idx;
    // 当前目录的后兄弟
    uint32_t nextSiblingDirectory_sector_idx;

    // 父目录
    DirectoryInfo parent_dir_info;

    // 获取当前目录
    read_sector(sector_idx);

    // 获取前后兄弟
    prevSiblingDirectory_sector_idx = ((DirectoryInfo *)fs_buffer)->prevSiblingDirectory_sector_idx;
    nextSiblingDirectory_sector_idx = ((DirectoryInfo *)fs_buffer)->nextSiblingDirectory_sector_idx;
    // 获取父目录
    read_sector(((DirectoryInfo *)fs_buffer)->parentDirectory_sector_idx);
    memcpy(&parent_dir_info, fs_buffer, sizeof(DirectoryInfo));

    // 判断是否是唯一的子目录
    if(prevSiblingDirectory_sector_idx == sector_idx &&
        nextSiblingDirectory_sector_idx == sector_idx) {
        // 情况1
        // 直接设置父目录的子目录为0就行了
        parent_dir_info.subdirectory_sector_idx = 0;

    } else {

        // 情况2和3统一的操作

        // 兄弟目录
        DirectoryInfo sibling_dir_info;

        // 取前兄弟结点，它的next直接接当前目录的next
        read_sector(prevSiblingDirectory_sector_idx);
        memcpy(&sibling_dir_info, fs_buffer, 512);
        sibling_dir_info.nextSiblingDirectory_sector_idx = nextSiblingDirectory_sector_idx;
        // 写出前兄弟结点到外存
        update_checksum(&sibling_dir_info);
        memcpy(fs_buffer, &sibling_dir_info, 512);
        write_sector(sibling_dir_info.sector_idx);

        // 取后兄弟结点，它的prev直接接当前目录的prev
        read_sector(nextSiblingDirectory_sector_idx);
        memcpy(&sibling_dir_info, fs_buffer, 512);
        sibling_dir_info.prevSiblingDirectory_sector_idx = prevSiblingDirectory_sector_idx;
        // 写出后兄弟结点到外存
        update_checksum(&sibling_dir_info);
        memcpy(fs_buffer, &sibling_dir_info, 512);
        write_sector(sibling_dir_info.sector_idx);

        if(parent_dir_info.subdirectory_sector_idx == sector_idx) {
            // 情况2的特殊操作，更换头结点
            // 让后兄弟取代这个位置
            parent_dir_info.subdirectory_sector_idx = nextSiblingDirectory_sector_idx;
        }
    }

    // 无论如何：原来父目录的计数器-1
    parent_dir_info.subdirectory_count--;
    // 写出原父目录
    update_checksum(&parent_dir_info);
    memcpy(fs_buffer, &parent_dir_info, 512);
    write_sector(parent_dir_info.sector_idx);

    return true;
}

// 复制该目录连带其子目录到目标目录下，该目录成为目标目录下的子目录，目标目录是新目录的父目录，返回操作是否成功
bool copy_directory(uint32_t src_sector_idx, uint32_t dest_sector_idx) {
    // 还没实现，目前还没想好
    // 怕编译器报错
    (void)src_sector_idx;
    (void)dest_sector_idx;
    return false;
}

// 读入目录信息到给定的存储目录信息的空间，参数是存储目录信息和目录信息所在扇区索引，返回值是是否读出一个有效的目录信息
bool read_directory(DirectoryInfo *dir_buffer, uint32_t sector_idx) {
    // 无效的目录信息，无法处理
    if (is_valid_directory(sector_idx) == false) {
        return false;
    }
    // 读取文件元信息
    read_sector(sector_idx);
    // 数据复制过去
    memcpy(dir_buffer, fs_buffer, sizeof(DirectoryInfo));
    return true;
}

// 写出给定的目录信息到指定的扇区，参数是存储目录信息的空间和目录信息所在扇区索引，返回值是是否写出成功
bool write_directory(DirectoryInfo *dir_buffer, uint32_t sector_idx) {

    // 检查给定的是不是一个有效的目录信息
    if (strcmp(dir_buffer->magic_header, "HCFileSystemDir") != 0) {
        return false;
    }
    if (strcmp(dir_buffer->magic_header2, "ThisDirIsValid_") != 0) {
        return false;
    }
    if (dir_buffer->sector_idx != sector_idx) {
        return false;
    }
    if (dir_buffer->checksum != calculate_checksum(dir_buffer)) {
        return false;
    }

    // 是一个有效的目录，可以写出
    memcpy(fs_buffer, dir_buffer, sizeof(DirectoryInfo));
    write_sector(sector_idx);

    return true;
}


// 校验是否是一个有效的文件元信息
bool is_valid_fileMetaInfo(uint32_t sector_idx) {
    if(sector_idx == 0) {
        return false;
    }
    if(is_used_sector(sector_idx) == false) {
        return false;
    }
    read_sector(sector_idx);
    FileMetaInfo *fmi = (FileMetaInfo *)fs_buffer;
    if(strcmp("HCFSFileMetaMgc", fmi->magic_header) == 0 && fmi->sector_idx == sector_idx) {
        return true;
    }
    return false;
}

// 读出文件元信息到给定的存储文件元信息的空间，参数是存储文件元信息的空间和文件元信息所在扇区，返回值是是否读出一个有效的文件元信息
bool file_read_fileMetaInfo(FileMetaInfo *fmi_buffer, uint32_t fileMetaSector_idx) {
    // 无效的文件元信息，无法处理
    if (is_valid_fileMetaInfo(fileMetaSector_idx) == false) {
        return false;
    }
    // 读取文件元信息
    read_sector(fileMetaSector_idx);
    FileMetaInfo *fmi = (FileMetaInfo *)fs_buffer;
    // 数据复制过去
    memcpy(fmi_buffer, fmi, sizeof(FileMetaInfo));
    return true;
}

// 写出给定的文件元信息到指定的扇区，参数是存储文件元信息的空间和文件元信息所在扇区，返回值是是否写出成功
bool file_write_fileMetaInfo(FileMetaInfo *fmi_buffer, uint32_t fileMetaSector_idx) {
    // 检查给定的是不是一个有效的文件元信息
    if(strcmp("HCFSFileMetaMgc", fmi_buffer->magic_header) != 0) {
        return false;
    }
    if(fmi_buffer->sector_idx != fileMetaSector_idx) {
        return false;
    }
    // 是一个有效的文件元信息，可以写出

    // 读出扇区数据到缓冲区
    read_sector(fileMetaSector_idx);
    // 把开头替换为指定的文件元信息
    memcpy(fs_buffer, fmi_buffer, sizeof(FileMetaInfo));
    // 其他数据不变，写出到外存
    write_sector(fileMetaSector_idx);
    return true;
}

// 常用操作：文件元信息目录计数器+1，传入参数是文件元信息所在的扇区索引，返回是操作是否成功
bool fileMetaInfo_directoryCounter_Increment(uint32_t fileMetaSector_idx) {
    // 无效的文件元信息，无法处理
    if (is_valid_fileMetaInfo(fileMetaSector_idx) == false) {
        return false;
    }
    // 读取文件元信息
    read_sector(fileMetaSector_idx);
    FileMetaInfo *fmi = (FileMetaInfo *)fs_buffer;
    // 数据变更
    fmi->directoryCounter++;
    // 写出
    write_sector(fileMetaSector_idx);
    return true;
}

// 常用操作：文件元信息目录计数器-1，传入参数是文件元信息所在的扇区索引，返回是操作是否成功
bool fileMetaInfo_directoryCounter_Decrement(uint32_t fileMetaSector_idx) {
    // 无效的文件元信息，无法处理
    if (is_valid_fileMetaInfo(fileMetaSector_idx) == false) {
        return false;
    }
    // 读取文件元信息
    read_sector(fileMetaSector_idx);
    FileMetaInfo *fmi = (FileMetaInfo *)fs_buffer;
    // 数据变更
    fmi->directoryCounter--;
    // 记录计数器
    uint32_t directoryCounter = fmi->directoryCounter;
    // 写出
    write_sector(fileMetaSector_idx);
    // 如果引用其的目录已经变成了0，尝试删除
    if (directoryCounter == 0) {
        return file_delete(fileMetaSector_idx);
    }

    return true;
}

// 校验这个扇区开头是否是一个有效的文件信息
bool is_valid_fileInfo(uint32_t sector_idx) {
    if(sector_idx == 0) {
        return false;
    }
    if(is_used_sector(sector_idx) == false) {
        return false;
    }
    read_sector(sector_idx);
    FileInfo *fileInfo = (FileInfo *)fs_buffer;
    if(fileInfo->magic_number == 0x66697331 && fileInfo->sector_idx == sector_idx) {
        return true;
    }
    return false;
}

// 文件创建，创建一个空文件的文件元信息，返回文件元信息所在的扇区
uint32_t file_create() {
    // 文件元信息结构体
    FileMetaInfo fmi;
    // 清空
    memset(&fmi, 0, sizeof(FileMetaInfo));
    // 申请扇区
    uint32_t sector_idx = acquire_file_sector(1);
    if(sector_idx == 0) {
        // 创建失败，什么都做不了
        return sector_idx;
    }
    // 创建成功就设置各项值
    // 刚创建什么都没有很正常
    // 计数器肯定是0，下一个扇区是0，末尾扇区也是0

    // 魔数头加上
    strcpy(fmi.magic_header, "HCFSFileMetaMgc");
    // 扇区索引加上
    fmi.sector_idx = sector_idx;
    // 计数器（文件元信息就占了一个扇区）
    fmi.sector_count = 1;
    // 时间戳加上
    fmi.creationTime = get_unix_timestamp_now();
    fmi.modificationTime = fmi.creationTime;
    // 文件crc32加上（什么都没有，初始值就是0xffffffff）
    fmi.fileCrc32 = 0xffffffff;
    // 其他都是0，直接写出到外存
    memcpy(fs_buffer, &fmi, sizeof(FileMetaInfo));
    // 写出到外存
    write_sector(fmi.sector_idx);
    // 返回fmi的扇区索引
    return fmi.sector_idx;
}

// 文件删除，返回是否成功，参数是文件元信息所在扇区
bool file_delete(uint32_t fileMetaSector_idx) {
    // 无效的文件元信息，无法处理
    if(is_valid_fileMetaInfo(fileMetaSector_idx) == false) {
        return false;
    }
    read_sector(fileMetaSector_idx);
    FileMetaInfo *fmi = (FileMetaInfo *)fs_buffer;
    if(fmi->directoryCounter > 0) {
        // 有目录还在使用这个文件元信息，不得删除
        return false;
    }
    // 把所有的位图置为未使用即可
    // 这里要用链表操作
    // 逐个release_file_sector

    // 记录文件元信息中的扇区计数和下一个文件扇区索引
    uint32_t sector_count = fmi->sector_count;
    uint32_t current_sector_idx = fmi->next_file_sector_idx;
    uint32_t next_sector_idx;

    // 释放文件元信息所在的扇区
    release_file_sector(fileMetaSector_idx, 1);

    // 循环释放文件数据扇区（i为1是因为文件元信息占据了一个扇区，而且已经被release了）
    for (uint32_t i = 1; i < sector_count; i++) {
        if (current_sector_idx == 0) {
            // 如果意外遇到链表末尾，退出循环
            break;
        }
        // 读取当前扇区的数据
        read_sector(current_sector_idx);
        FileInfo *file_info = (FileInfo *)fs_buffer;
        // 获取下一个文件扇区索引
        next_sector_idx = file_info->next_file_sector_idx;
        // 释放当前扇区
        release_file_sector(current_sector_idx, 1);
        // 移动到下一个扇区
        current_sector_idx = next_sector_idx;
    }

    return true;
}

// 读入文件数据到给定的buffer，参数是文件元信息所在扇区索引，开始读取位置，buffer，读取长度，返回值是读出数据的长度（以字节计算）
uint32_t file_read_context(uint32_t fileMetaSector_idx, uint32_t pos, void *buffer, uint32_t len) {
    if (!is_valid_fileMetaInfo(fileMetaSector_idx)) {
        return 0; // 无效的文件元信息，无法处理
    }

    // 读取文件元信息
    read_sector(fileMetaSector_idx);
    FileMetaInfo *fmi = (FileMetaInfo *)fs_buffer;

    // 检查读取位置是否超出文件长度
    if (pos >= fmi->fileLength) {
        return 0; // 起始位置超出文件长度，无法读取
    }

    // 调整读取长度，确保不会超出文件末尾
    if (pos + len > fmi->fileLength) {
        len = fmi->fileLength - pos;
    }

    // 当前读取的位置和长度
    uint32_t current_pos = pos;
    // 还要读多少字节
    uint32_t bytes_to_read = len;
    uint32_t bytes_read = 0;
    uint8_t *buffer_ptr = (uint8_t *)buffer;

    // 找出第一个文件扇区
    uint32_t current_sector_idx = fmi->next_file_sector_idx;
    // 下一个文件扇区
    uint32_t next_sector_idx;
    // 一共多少个扇区
    uint32_t total_sector_count = fmi->sector_count;

    // 开始读取文件数据
    // 用for循环记录现在读的第几个扇区
    for (uint32_t current_sector = 1; current_sector < total_sector_count && bytes_to_read > 0; current_sector++) {
        // 校验是否是一个文件扇区
        if( !is_valid_fileInfo(current_sector_idx) ) {
            // 如果不是，直接返回已读的字节数
            return bytes_read;
        }
        // 确实是一个文件扇区
        read_sector(current_sector_idx);
        FileInfo *file_info = (FileInfo *)fs_buffer;

        // 读入到缓冲区之后先更新下标
        next_sector_idx = file_info->next_file_sector_idx;

        // 判断是否在这个扇区内（就是current_pos是否在这个扇区内）
        // 因为之前计算好了，一个扇区有512个字节，一个文件扇区前12个字节都给了记录信息，后500字节是数据
        // 假设扇区开头是0，那么结束位置就是499，一共500个
        // 此时current_sector = 1，在那么500就是下一个扇区的位置
        // 因此current_pos >= 0 且 current_pos < 500，那么就在这个扇区中
        // 因为都是从0开始，所以只需要判断current_pos < 500即可命中

        // 所以写条件 current_pos < current_sector * 500
        // 记住current_pos要变动

        if(current_pos < current_sector * 500) {
            // 在这个扇区范围内
            // 先计算偏移量和读取数量
            uint32_t sector_data_offset = current_pos % 500;
            uint32_t sector_bytes_to_read = 500 - sector_data_offset;
            // 防止最后一个扇区的特殊情况（数据不满整个扇区）
            if (sector_bytes_to_read > bytes_to_read) {
                sector_bytes_to_read = bytes_to_read;
            }
            // 从这个偏移量开始读到末尾
            memcpy(buffer_ptr, file_info->data + sector_data_offset, sector_bytes_to_read);
            // 指针步进
            buffer_ptr += sector_bytes_to_read;
            // 读取位置变动
            current_pos += sector_bytes_to_read;
            // 已经读取的字节数变动
            bytes_read += sector_bytes_to_read;
            // 还要读多少字节
            bytes_to_read -= sector_bytes_to_read;
        }

        // 读取完毕，步进到下一个扇区
        current_sector_idx = next_sector_idx;
    }

    return bytes_read;
}

// 将给定的数据写出到文件某个位置之后，参数是文件元信息所在扇区索引，开始写出位置（注意，该位置之后的数据将会永久丢失），buffer，读取长度，返回值是读出数据的长度（以字节计算）
uint32_t file_write_context(uint32_t fileMetaSector_idx, uint32_t pos, void *buffer, uint32_t len) {
    if (!is_valid_fileMetaInfo(fileMetaSector_idx)) {
        return 0; // 无效的文件元信息，无法处理
    }

    // 读取文件元信息
    FileMetaInfo fmi;
    read_sector(fileMetaSector_idx);
    memcpy(&fmi, fs_buffer, sizeof(FileMetaInfo));

    // 不得越界写
    if(pos > fmi.fileLength) {
        return 0;
    }

    // 当前写入的位置和长度
    uint32_t current_pos = pos;
    uint32_t bytes_to_write = len;
    uint32_t bytes_written = 0;
    uint8_t *buffer_ptr = (uint8_t *)buffer;

    // 找出第一个文件扇区
    uint32_t current_sector_idx = fmi.next_file_sector_idx;

    // 通用逻辑，其实可以特化处理写入文件末尾的情况，能提高不少效率

    // 文件扇区数据（用于保存当前的信息）
    FileInfo fileInfo;
    // 上一个扇区的索引
    uint32_t prev_file_sector_idx = 0;
    // 记录现在是第几个扇区
    uint32_t current_sector = 1;

    // 写入数据要用while循环，因为不知道有多长
    while(bytes_to_write > 0) {

        // 判断要不要申请一个文件扇区
        if(current_sector_idx == 0) {
            // 相当于写入空文件了，申请一个扇区
            uint32_t sector_idx = acquire_file_sector(1);
            if(sector_idx == 0) {
                // 申请不了，直接return
                return bytes_written;
            }
            // 清空缓冲区，直接写出
            memset(fs_buffer, 0, sizeof(FileInfo));
            ((FileInfo *)fs_buffer)->magic_number = 0x66697331;
            ((FileInfo *)fs_buffer)->sector_idx = sector_idx;
            write_sector(sector_idx);

            // 如果前一个是0，说明是写入空文件
            if(prev_file_sector_idx == 0) {
                fmi.next_file_sector_idx = sector_idx;
            } else {
                // 接续链表
                // 读入直接写出，节省时间
                read_sector(prev_file_sector_idx);
                ((FileInfo *)fs_buffer)->next_file_sector_idx = sector_idx;
                write_sector(prev_file_sector_idx);
            }
            // 统一操作，fmi的扇区计数器增加
            fmi.sector_count++;
            // 更新current_sector_idx
            current_sector_idx = sector_idx;
        }

        if(!is_valid_fileInfo(current_sector_idx)) {
            // 碰上这种情况说明文件损坏
            return bytes_written;
        }

        // 读入扇区
        read_sector(current_sector_idx);
        memcpy(&fileInfo, fs_buffer, sizeof(FileInfo));

        // 仿照read的逻辑，只要 current_pos < current_sector * 500 即可命中
        if(current_pos < current_sector * 500) {
            // 在这个扇区范围内
            // 先计算偏移量和写出数量
            uint32_t sector_data_offset = current_pos % 500;
            uint32_t sector_bytes_to_write = 500 - sector_data_offset;
            // 防止最后一个扇区的特殊情况（数据不满整个扇区）
            if (sector_bytes_to_write > bytes_to_write) {
                sector_bytes_to_write = bytes_to_write;
            }
            // 之前扇区已经被读入到fileInfo中了，修改fileInfo的内容即可
            // 从这个偏移量开始读到末尾
            memcpy(fileInfo.data + sector_data_offset, buffer_ptr, sector_bytes_to_write);
            // 写出到外存
            memcpy(fs_buffer, &fileInfo, sizeof(FileInfo));
            write_sector(fileInfo.sector_idx);
            // 指针步进
            buffer_ptr += sector_bytes_to_write;
            // 读取位置变动
            current_pos += sector_bytes_to_write;
            // 已经读取的字节数变动
            bytes_written += sector_bytes_to_write;
            // 还要读多少字节
            bytes_to_write -= sector_bytes_to_write;
        }

        // 更新索引
        prev_file_sector_idx = current_sector_idx;
        current_sector_idx = fileInfo.next_file_sector_idx;
        current_sector++;
    }

    // 更新文件元信息到外存
    fmi.fileLength = current_pos;
    fmi.modificationTime = get_unix_timestamp_now();
    // 写出文件元信息（然后才能更新crc32）
    memcpy(fs_buffer, &fmi, sizeof(FileMetaInfo));
    write_sector(fmi.sector_idx);

    return bytes_written;
}

// 将目录转变为文件，该目录不能有子目录，否则会被认为是目录而非文件（其实就是修改，把文件元信息扇区加上去而已）
bool convert_dir_to_file(uint32_t dir_sector_idx, uint32_t fileMetaSector_idx) {
    if(!is_valid_directory(dir_sector_idx)) {
        // 目录无效不操作
        return false;
    }
    if(!is_valid_fileMetaInfo(fileMetaSector_idx)) {
        // 文件元信息无效不操作
        return false;
    }
    read_sector(dir_sector_idx);
    if(((DirectoryInfo *)fs_buffer)->subdirectory_count != 0 || ((DirectoryInfo *)fs_buffer)->subdirectory_sector_idx != 0) {
        // 有子目录说明这是个目录，不能成为文件
        return false;
    }
    ((DirectoryInfo *)fs_buffer)->fileMetadata_sector_idx = fileMetaSector_idx;
    write_sector(dir_sector_idx);
    return true;
}


// 下面是通过查表法速算crc32摘要算法

// 计算文件的crc32，参数是文件元信息扇区（直接更新写入外存）
void fileCrc32(uint32_t fileMetaSector_idx){

    FileMetaInfo fmi;
    read_sector(fileMetaSector_idx);
    memcpy(&fmi, fs_buffer, sizeof(FileMetaInfo));

    // 初始化crc32表
    uint32_t crc32_table[256];
    uint32_t polynomial = 0xedb88320;
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t temp_crc = i;
        for (uint32_t j = 8; j > 0; j--) {
            if (temp_crc & 1) {
                temp_crc = (temp_crc >> 1) ^ polynomial;
            } else {
                temp_crc = temp_crc >> 1;
            }
        }
        crc32_table[i] = temp_crc;
    }

    // 初始化 CRC32 值
    uint32_t crc = 0xffffffff;

    // 获取第一个文件扇区索引
    uint32_t current_sector_idx = fmi.next_file_sector_idx;
    // 需要计算的字符数
    uint32_t count = fmi.fileLength;

    // 遍历文件扇区链表
    while (current_sector_idx != 0) {
        // 校验文件扇区
        if (!is_valid_fileInfo(current_sector_idx)) {
            return 0xffffffff; // 无效的文件扇区
        }

        // 读取文件扇区
        read_sector(current_sector_idx);
        FileInfo *file_info = (FileInfo *)fs_buffer;

        // 计算当前扇区数据的 CRC32
        // 每个文件扇区都有500字节的存储单位
        for (uint32_t i = 0; i < 500 && count > 0; i++, count--) {
            uint8_t byte = file_info->data[i];
            uint32_t lookupIndex = (crc ^ byte) & 0xff;
            crc = (crc >> 8) ^ crc32_table[lookupIndex];
        }

        // 获取下一个文件扇区索引
        current_sector_idx = file_info->next_file_sector_idx;
    }

    // 更新最终的 crc32 值到文件元数据
    fmi.fileCrc32 = crc ^ 0xffffffff;
    // 写到外存
    memcpy(fs_buffer, &fmi, sizeof(FileMetaInfo));
    write_sector(fmi.sector_idx);
}
