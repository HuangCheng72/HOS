//
// Created by huangcheng on 2024/6/19.
//

// 这个设备是内置在CPU里面的，根本不需要初始化也不需要退出

#include "rtc_cmos.h"
#include "../../kernel/kernel_device/kernel_device.h"

// 注册宏，定义一个驱动结构体，把驱动结构体实例放到驱动段
REGISTER_DRIVER(rtc_cmos_driver) {
        .driver_name = "rtc_cmos",
        .init = NULL,
        .exit = NULL,
        .irq = -1, // 没有中断处理
};

#define CMOS_ADDRESS    0x70
#define CMOS_DATA       0x71

static inline uint8_t cmos_read(uint8_t reg) {
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

static inline void cmos_write(uint8_t reg, uint8_t value) {
    outb(CMOS_ADDRESS, reg);
    outb(CMOS_DATA, value);
}

uint8_t bcd_to_bin(uint8_t bcd) {
    return ((bcd / 16) * 10) + (bcd & 0xf);
}

void rtc_get_time(rtc_time_t *time) {
    time->second = bcd_to_bin(cmos_read(0x00));
    time->minute = bcd_to_bin(cmos_read(0x02));
    time->hour = bcd_to_bin(cmos_read(0x04));
    time->day = bcd_to_bin(cmos_read(0x07));
    time->month = bcd_to_bin(cmos_read(0x08));
    time->year = bcd_to_bin(cmos_read(0x09)) + 2000;  // Assuming the RTC gives year as two-digit BCD
}

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

// 取现行时间戳
uint32_t get_unix_timestamp_now(void) {
    rtc_time_t time;
    rtc_get_time(&time);
    return get_unix_timestamp(&time);
}
