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

    // 用虚拟地址重新加载GDT
    load_gdt(32 * 8 - 1, GDT_BASE_ADDR + HIGH_ADDR_OFFSET);

}
