//
// Created by huangcheng on 2024/5/23.
//

#ifndef HOS_TYPE_H
#define HOS_TYPE_H

// 移植自stdint.h

typedef signed char             int8_t;
typedef unsigned char           uint8_t;
typedef short int               int16_t;
typedef unsigned short int      uint16_t;
typedef int                     int32_t;
typedef unsigned int            uint32_t;
typedef long long int           int64_t;
typedef unsigned long long int  uint64_t;

// 常用的一个表示
typedef uint64_t size_t;
// 常用的NULL表示
#define NULL (void *)(0)

#define INT8_MIN  (-128)
#define INT16_MIN (-32768)
#define INT32_MIN (-2147483647 - 1)
#define INT64_MIN (-9223372036854775807LL - 1)

#define INT8_MAX  127
#define INT16_MAX 32767
#define INT32_MAX 2147483647
#define INT64_MAX 9223372036854775807LL

#define UINT8_MAX  255
#define UINT16_MAX 65535
#define UINT32_MAX 0xffffffffU  /* 4294967295U */
#define UINT64_MAX 0xffffffffffffffffULL /* 18446744073709551615ULL */

// 用枚举定义的布尔值
typedef enum {
    false = 0,
    true
}bool;

#endif //HOS_TYPE_H
