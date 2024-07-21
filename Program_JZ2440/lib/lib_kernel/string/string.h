//
// Created by huangcheng on 2024/5/28.
//

#ifndef HOS_STRING_H
#define HOS_STRING_H

#include "../../type.h"

void memset(void* dst_, uint8_t value, uint32_t size);
void memcpy(void* dst_, const void* src_, uint32_t size);
int memcmp(const void* a_, const void* b_, uint32_t size);
char* strcpy(char* dst_, const char* src_);
uint32_t strlen(const char* str);
int8_t strcmp (const char *a, const char *b);
char* strchr(const char* string, const uint8_t ch);
char* strrchr(const char* string, const uint8_t ch);
char* strcat(char* dst_, const char* src_);
uint32_t strchrs(const char* filename, uint8_t ch);

// sprintf及其使用的转换函数
int sprintf(char* buffer, const char* format, ...);
void int_to_str(int num, char *buffer);
void uint_to_str(unsigned int num, char *buffer);
void hex_to_str(unsigned int num, char *buffer);
void ptr_to_str(void* ptr, char* buffer);

// 浮点数无法使用，原因看后面
// void double_to_str(double num, char* buffer, int precision);

#endif //HOS_STRING_H
