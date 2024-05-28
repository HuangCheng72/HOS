//
// Created by huangcheng on 2024/5/23.
//

#ifndef HOS_LIB_H
#define HOS_LIB_H

#include "../type.h"
#include "string/string.h"

// 这个文件存在的意义主要就是为了声明汇编暴露的函数，加上type.h可以为系统各组件提供统一的支持

// 载入页目录，load_page_directory.asm
extern void load_page_directory(uint32_t* page_directory);
// 启用分页，enable_paging.asm
extern void enable_paging();
// 加载GDT，load_gdt.asm
extern void load_gdt(uint16_t limit, uint32_t base);
// 设置esp，switch_esp_virtual_addr.asm
extern void switch_esp_virtual_addr();
// 将一个字符打印在光标处，print.asm
void put_char(char c);
//在光标位置打印字符串（长度不要超过屏幕一行能容纳的最多字符数80），print.asm
void put_str(const char *str);
//将整数以16进制形式打印在屏幕上，不打印前缀0x。print.asm
void put_int(int n);
// 加载IDT，load_idt.asm
extern void load_idt(uint16_t limit, uint32_t base);
// 向端口port写入一个字节的数据。io.asm
extern void outb(uint16_t port, uint8_t data);
// 将addr处起始的word_cnt个字写入端口port。io.asm
extern void outsw(uint16_t port, const void* addr, uint32_t word_cnt);
// 将从端口port读入的一个字节返回。io.asm
extern uint8_t inb(uint16_t port);
// 将从端口port读入的word_cnt个字写入addr。io.asm
extern void insw(uint16_t port, void* addr, uint32_t word_cnt);

#endif //HOS_LIB_H
