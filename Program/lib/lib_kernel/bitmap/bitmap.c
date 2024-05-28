//
// Created by huangcheng on 2024/5/28.
//

#include "bitmap.h"
#include "../string/string.h"

// 位图一律初始化，所有位全部置0
void bitmap_init(BitMap* btmp) {
    memset(btmp->bits, 0, btmp->btmp_bytes_len);
}

// 判断bit_idx位是否为1（即已经被设置）
int32_t bitmap_is_bit_set(BitMap* btmp, uint32_t bit_idx) {
    /*
    uint32_t byte_idx = bit_idx / 8;    // 向下取整用于索引数组下标
    uint32_t bit_odd  = bit_idx % 8;    // 取余用于索引数组内的位
    // 核心就是被检测位的位与运算
    // 位与数是1左移到那个位，比如1左移3位，二进制表示为00001000
    // 其他位和这个数的位与结果肯定都是0，被检测位如果是0那就自然是0，否则就是1
    int32_t result = (btmp->bits[byte_idx] & (1 << bit_odd));
    */

    if (bit_idx >= btmp->btmp_bytes_len * 8) {
        // 越界了，无论如何都是1
        return 1;  // 越界处理
    }

    return (btmp->bits[bit_idx / 8] & (1 << (bit_idx % 8)));
}

// 将位图btmp的bit_idx位设置为value
void bitmap_set(BitMap* btmp, uint32_t bit_idx, int8_t value) {
    ASSERT((value == 0) || (value == 1));

    if (bit_idx >= btmp->btmp_bytes_len * 8) {
        return;  // 越界处理
    }

    // 核心逻辑，如果设置数是0，那么直接对那一位位与就行了
    // 但是怕误伤其他位，所以参与位于的数据，其他位的位于必须是1，只能有该位是0
    // 假设还是和检测一样，位与数是1左移到那个位，比如1左移3位，二进制表示为00001000
    // 只要按位取反就好了，就可以得到11110111，正好符合我们的要求，按位取反运算符是波浪号

    // 如果设置数是1就要用位或了，直接00001000，该位位或，位或符号是单竖线，普通的或是双竖线

    if (value) {
        btmp->bits[bit_idx / 8] = btmp->bits[bit_idx / 8] | (1 << (bit_idx % 8));
    } else {
        btmp->bits[bit_idx / 8] = btmp->bits[bit_idx / 8] & (~(1 << (bit_idx % 8)));
    }
}

// 在位图中申请连续cnt个位,成功则返回其起始位下标，失败返回-1
int32_t bitmap_scan(BitMap* btmp, uint32_t cnt) {
    uint32_t idx_byte = 0;	 // 用于记录空闲位所在的字节
    // 首先逐个字节比较，如果值为0xff（255）意味着全部占满，那就没必要检测了，直接计数跳过即可
    while (( 0xff == btmp->bits[idx_byte]) && (idx_byte < btmp->btmp_bytes_len)) {
        idx_byte++;
    }

    if (idx_byte >= btmp->btmp_bytes_len) {
        // 若找不到可用空间，那就真没办法了
        return -1;
    }

    // 找到一个空闲位的字节，逐位比较，查找连续的空闲位
    int32_t bit_idx_start = -1;
    uint32_t count = 0;
    uint32_t bit_left = btmp->btmp_bytes_len * 8;  // 总位数

    for (uint32_t i = idx_byte * 8; i < bit_left; i++) {
        if ( bitmap_is_bit_set(btmp, i) ) {
            // 还没找到足够的空位就被中断了，重置计数器和起点
            bit_idx_start = -1;
            count = 0;

        } else {
            // 找到第一个空闲位
            if (bit_idx_start == -1) {
                bit_idx_start = i;
            }
            count++;
            if (count == cnt) {
                return bit_idx_start;  // 找到连续的cnt个空闲位
            }

        }
    }
    return -1;  // 找不到连续的cnt个空闲位
}

void bitmap_set_range(BitMap* btmp, uint32_t start_idx, uint32_t cnt, int8_t value) {
    if (start_idx + cnt > btmp->btmp_bytes_len * 8) {
        return;  // 越界处理
    }
    if (cnt == 0) {
        return;
    }

    ASSERT((value == 0) || (value == 1));

    uint32_t byte_idx = start_idx / 8;
    uint32_t bit_idx = start_idx % 8;
    uint32_t bits_left = cnt;

    // 处理第一个字节中的剩余位
    for(;bits_left > 0 && bit_idx < 8; bit_idx++){
        bitmap_set(btmp, byte_idx * 8 + bit_idx, value);
        bits_left--;
    }

    // 如果第一个字节处理完毕，更新 byte_idx
    if (bit_idx == 8) {
        byte_idx++;
        bit_idx = 0;
    }

    // 处理完整的字节
    while (bits_left >= 8) {
        btmp->bits[byte_idx] = value ? 0xff : 0x00;
        bits_left -= 8;
        byte_idx++;
    }

    // 处理最后一个字节中的剩余位
    while (bits_left > 0) {
        bitmap_set(btmp, byte_idx * 8 + bit_idx, value);
        bits_left--;
        bit_idx++;
    }
}
