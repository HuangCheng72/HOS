//
// Created by huangcheng on 2024/6/3.
//

#ifndef HOS_CONSOLE_H
#define HOS_CONSOLE_H

// 这是个虚拟设备
// 不是实际存在的设备，只是为了保证同步抽象出来的

// 终端打印字符串
void console_put_str(const char* str);

// 终端打印字符
void console_put_char(char char_asci);

// 终端打印16进制整数
void console_put_int(int num);


#endif //HOS_CONSOLE_H
