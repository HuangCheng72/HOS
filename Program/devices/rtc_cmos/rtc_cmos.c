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

uint32_t get_unix_timestamp(rtc_time_t *time) {
    // Convert rtc_time_t to Unix timestamp
    // This is a simplified example and might not handle leap years and other details correctly
    // A more robust implementation would be needed for production use

    uint32_t days = (time->year - 1970) * 365 + (time->year - 1969) / 4;
    days += (time->month - 1) * 30 + time->day;

    return days * 86400 + time->hour * 3600 + time->minute * 60 + time->second;
}

// 取现行时间戳
uint32_t _time_get(void) {
    rtc_time_t time;
    rtc_get_time(&time);
    return get_unix_timestamp(&time);
}
