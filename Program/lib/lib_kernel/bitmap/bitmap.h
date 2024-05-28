//
// Created by huangcheng on 2024/5/28.
//

#ifndef HOS_BITMAP_H
#define HOS_BITMAP_H

#include "../../type.h"

typedef struct {
    // 位图长度（字节数）
    uint32_t btmp_bytes_len;
    // 位图起始地址
    uint8_t* bits;
}BitMap;

// 位图初始化
void bitmap_init(BitMap* btmp);
// 判断bit_idx位是否为1（即已经被设置）
int32_t bitmap_is_bit_set(BitMap* btmp, uint32_t bit_idx);
// 将位图btmp的bit_idx位设置为value
void bitmap_set(BitMap* btmp, uint32_t bit_idx, int8_t value);
// 在位图中申请连续cnt个位,成功则返回其起始位下标，失败返回-1
int32_t bitmap_scan(BitMap* btmp, uint32_t cnt);
// 连续设置位图
void bitmap_set_range(BitMap* btmp, uint32_t start_idx, uint32_t cnt, int8_t value);

#endif //HOS_BITMAP_H
