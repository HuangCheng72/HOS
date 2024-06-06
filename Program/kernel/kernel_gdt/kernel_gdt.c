//
// Created by huangcheng on 2024/5/23.
//

#include "kernel_gdt.h"

void setup_gdt() {

    // GDT表所在位置
    SegmentDescriptor *gdt = (SegmentDescriptor *)GDT_BASE_ADDR;

    // C语言编程惯例，用指针改，才能接管控制权，直接改内存，否则改不了
    // 改的是第四个描述符 视频段
    (gdt + 3)->limit_low = 0x7fff;
    (gdt + 3)->base_low = 0x8000;
    (gdt + 3)->base_mid = 0x0B;
    (gdt + 3)->type = DESC_TYPE_DATA;
    (gdt + 3)->s = DESC_S_CODE_OR_DATA;
    (gdt + 3)->dpl = DESC_DPL0;
    (gdt + 3)->p = DESC_PRESENT;
    (gdt + 3)->limit_high = 0;
    (gdt + 3)->avl = DESC_AVL;
    (gdt + 3)->l = DESC_LONG_MODE;
    (gdt + 3)->db = DESC_DB;
    (gdt + 3)->g = DESC_G_4K;
    (gdt + 3)->base_high = 0xc0;    // 加上偏移量0xc0000000，从虚拟内存可以映射到物理内存，这也是调整视频段的基址

    // GDT搬家
    SegmentDescriptor *new_gdt = (SegmentDescriptor *)NEW_GDT_BASE_ADDR;
    *(uint64_t *)(new_gdt + 0) = *(uint64_t *)(gdt + 0);
    *(uint64_t *)(new_gdt + 1) = *(uint64_t *)(gdt + 1);
    *(uint64_t *)(new_gdt + 2) = *(uint64_t *)(gdt + 2);
    *(uint64_t *)(new_gdt + 3) = *(uint64_t *)(gdt + 3);

    // 第四个放TSS段，这是所有任务共用的tss
    // 先清理干净TSS的位置
    memset((void *)TSS_ADDR, 0, sizeof(struct TSS));
    // 放内核态各个段寄存器的段选择子
    ((struct TSS *)TSS_ADDR)->cs = SELECTOR_K_CODE;

    ((struct TSS *)TSS_ADDR)->ds = SELECTOR_K_DATA;
    ((struct TSS *)TSS_ADDR)->es = SELECTOR_K_DATA;
    ((struct TSS *)TSS_ADDR)->fs = SELECTOR_K_DATA;
    ((struct TSS *)TSS_ADDR)->gs = SELECTOR_K_VIDEO;

    ((struct TSS *)TSS_ADDR)->ss = SELECTOR_K_STACK;

    // 内核态当然用内核堆栈段，其实也就是数据段
    ((struct TSS *)TSS_ADDR)->ss0 = SELECTOR_K_STACK;
    // 这是禁用IO位图
    ((struct TSS *)TSS_ADDR)->io_map_base_address = sizeof(struct TSS);

    // 因为现在还没有用户进程，所有的任务都是内核任务，都要运行在内核态
    // 所以特权级（DPL）是0
    // TSS段的性质类似于代码段
    new_gdt[4].base_low = ((TSS_ADDR) & 0xffff);                  // 段基址低16位
    new_gdt[4].base_mid = (((TSS_ADDR) >> 16) & 0xff);            // 段基址中间8位
    new_gdt[4].base_high = (((TSS_ADDR) >> 24) & 0xff);           // 段基址高8位
    new_gdt[4].limit_low = 0x67;            // 段界限低16位（104的16进制就是0x68，0x68 - 1 = 0x67）
    new_gdt[4].limit_high = 0x0;            // 段界限高16位
    new_gdt[4].type = 9;                    // 类型
    new_gdt[4].s = 0;                       // 描述符类型 (系统段)
    new_gdt[4].dpl = 0;                     // 特权级 0
    new_gdt[4].p = 1;                       // 段存在位
    new_gdt[4].avl = 0;                     // 可用位
    new_gdt[4].l = 0;                       // 64 位代码段
    new_gdt[4].db = 0;                      // 默认操作大小
    new_gdt[4].g = 1;                       // 粒度 (4KB)

    // 用户态的代码段和数据段和内核态的代码段和数据段，只有特权级dpl的不同。只需要修改这两个就可以了。
    // 用户态的代码段，先复制过来，然后修改
    *(uint64_t *)(new_gdt + 5) = *(uint64_t *)(new_gdt + 1);
    // 改成最低特权级就行了
    new_gdt[5].dpl = DESC_DPL3;
    // 用户态数据段一样
    *(uint64_t *)(new_gdt + 6) = *(uint64_t *)(new_gdt + 2);
    new_gdt[6].dpl = DESC_DPL3;

    // 用虚拟地址重新加载GDT
    load_gdt(32 * 8 - 1, NEW_GDT_BASE_ADDR + HIGH_ADDR_OFFSET);
    // 加载tss
    load_tss(SELECTOR_TSS);
    // 顺便清理一下原来的gdt，方便给内核作为栈使用
    memset(gdt, 0, 32 * 8 - 1);

}
