//
// Created by huangcheng on 2024/6/27.
//

#ifndef HOS_PRINT_H
#define HOS_PRINT_H

// 将一个字符打印在光标处
void put_char(char c);
//在光标位置打印字符串（长度不要超过屏幕一行能容纳的最多字符数80）
void put_str(const char *str);
//将整数以16进制形式打印在屏幕上，不打印前缀0x。
void put_int(int n);

#endif //HOS_PRINT_H
