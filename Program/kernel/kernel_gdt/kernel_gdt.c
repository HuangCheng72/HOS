//
// Created by huangcheng on 2024/5/23.
//

#include "kernel_gdt.h"

void setup_gdt() {

    // 再把GDT的值存储好
    store_gdt(GDT_PTR_ADDR);

    // GDT表所在位置
    SegmentDescriptor *gdt = (SegmentDescriptor *)GDT_BASE_ADDR;

    // C语言编程惯例，用指针改，才能接管控制权，直接改内存，否则改不了
    // 改的是第四个描述符 视频段
    (gdt + 3)->limit_low = 0x7fff;
    (gdt + 3)->base_low = 0x8000;
    (gdt + 3)->base_mid = 0x0B;
    (gdt + 3)->access = (0x92); // 数据段, DPL=0, 存在位=1
    (gdt + 3)->granularity = (DESC_G_4K | DESC_D_32);
    (gdt + 3)->base_high = 0xc0;    // 加上偏移量0xc0000000，从虚拟内存可以映射到物理内存，这也是调整视频段的基址

    // 设置GDT指针
    GdtPtr *gdt_ptr = (GdtPtr *)(GDT_PTR_ADDR);
    // limit就别动它了，用不上
    gdt_ptr->base += HIGH_ADDR_OFFSET;   // 基地址加上0xC0000000

    // 用虚拟地址重新加载GDT
    load_gdt(GDT_PTR_ADDR + HIGH_ADDR_OFFSET);

}
