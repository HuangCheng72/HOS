//
// Created by huangcheng on 2024/6/19.
//

// arm上用的是PL031，看设备树可以看到

#include "rtc_cmos.h"
#include "../../kernel/kernel_device/kernel_device.h"

// 注册宏，定义一个驱动结构体，把驱动结构体实例放到驱动段
REGISTER_DRIVER(rtc_cmos_driver) {
        .driver_name = "rtc_cmos",
        .init = NULL,
        .exit = NULL,
        .irq = -1, // 没有中断处理
};

#define PL031_BASE_ADDR 0x09010000
#define PL031_DR (*(volatile uint32_t *)(PL031_BASE_ADDR)) // 数据寄存器地址（读这个地址，会提供当前时间戳）

// 每个月的天数数组
static const uint8_t days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// 将RTC时间转换为时间戳
uint32_t get_unix_timestamp(rtc_time_t *time) {
    uint32_t days = 0;
    uint32_t year = time->year;
    uint32_t month = time->month;
    uint32_t day = time->day;

    // 计算从1970年到当前年份的总天数
    for (uint32_t y = 1970; y < year; y++) {
        days += (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0)) ? 366 : 365;
    }

    // 计算当前年份中从1月到当前月份的总天数
    for (uint32_t m = 1; m < month; m++) {
        days += days_in_month[m - 1];
        if (m == 2 && (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))) {
            days += 1; // 闰年二月增加一天
        }
    }

    // 加上当前月份的天数
    days += day - 1;

    // 计算总秒数
    uint32_t total_seconds = days * 86400;
    total_seconds += time->hour * 3600;
    total_seconds += time->minute * 60;
    total_seconds += time->second;

    return total_seconds;
}

// 判断是否是闰年
static int is_leap_year(int year) {
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

// 将UNIX时间戳转换为RTC时间
void unix_to_rtc_time(uint32_t timestamp, rtc_time_t *time) {
    uint32_t days = timestamp / 86400;
    uint32_t remaining_seconds = timestamp % 86400;

    time->hour = remaining_seconds / 3600;
    remaining_seconds %= 3600;
    time->minute = remaining_seconds / 60;
    time->second = remaining_seconds % 60;

    // 从1970年1月1日开始计算
    uint32_t year = 1970;
    while (days >= (is_leap_year(year) ? 366 : 365)) {
        days -= is_leap_year(year) ? 366 : 365;
        year++;
    }
    time->year = year;

    uint32_t month = 1;
    while (days >= days_in_month[month - 1]) {
        if (month == 2 && is_leap_year(year)) {
            if (days >= 29) {
                days -= 29;
            } else {
                break;
            }
        } else {
            days -= days_in_month[month - 1];
        }
        month++;
    }
    time->month = month;
    time->day = days + 1;
}

// 取现行时间戳
uint32_t get_unix_timestamp_now(void) {
    uint32_t res = PL031_DR;
    return res;
}
