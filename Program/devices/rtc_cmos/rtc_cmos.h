//
// Created by huangcheng on 2024/6/19.
//

#ifndef HOS_RTC_CMOS_H
#define HOS_RTC_CMOS_H

#include "../../lib/lib_kernel/lib_kernel.h"

// rtc时间结构体
typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} rtc_time_t;

// RTC CMOS 获取当前时间
void rtc_get_time(rtc_time_t *time);
// 将RTC时间转换为时间戳
uint32_t get_unix_timestamp(rtc_time_t* time);
// 取现行时间戳
uint32_t get_unix_timestamp_now(void);

#endif //HOS_RTC_CMOS_H
