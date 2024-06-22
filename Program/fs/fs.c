//
// Created by huangcheng on 2024/6/21.
//

#include "fs.h"
#include "fs_int.h"

// 本来这部分也属于hl
// 我是觉得写在同一个文件里面太多了

// 文件系统启动
int16_t fs_start(void) {
    // 读入超级块
    // 正常情况下这里分很多模式，比如检查，扫盘之类的，Linux在这方面卡得很严
    // 我这里没那么多讲究，我连盘都是虚拟的，直接读超级块就得了

    // 判断超级块是否有效
    // 如果超级块无效的话，要格式化

    if(is_valid_superblock()) {
        read_superblock();
        return 0;
    } else {
        return fs_format(FS_FORMAT_SOFT);
    }
}
// 文件系统退出
int16_t fs_exit(void) {
    // 要把超级块写出到外存
    write_superblock();
    return 0;
}
// 格式化文件系统
int16_t fs_format(uint8_t fmode) {
    if(fmode == FS_FORMAT_FULL) {
        // 完全格式化要清盘，太浪费时间了，不模仿了

        // 传入多少个扇区，初始化超级块
        init_superblock(DISK_SIZE / 512);
        // 建立根目录
        create_root_directory();
        // 写出超级块
        write_superblock();

        return 0;
    }

    if(fmode == FS_FORMAT_SOFT) {
        // 软格式化就是重建超级块和位图而已

        // 传入多少个扇区，初始化超级块
        init_superblock(DISK_SIZE / 512);
        // 建立根目录
        create_root_directory();
        // 写出超级块
        write_superblock();

        return 0;
    }
    return -1;
}

// 打开文件
int16_t fs_open(FS_DESC *pdesc, char* pname) {
    if(strlen(pname) == 0) {
        return -1;
    }
    if(pname[0] != '/') {
        // 路径必须是/开头的绝对路径
        return -1;
    }
    // 解析路径
    char dir_name[FS_FILENAME_MAX_LEN + 1] = { 0 };
    // 因为第一个字符肯定是/，所以从第二个字符开始扫
    uint32_t idx_start = 0, idx_end = 1 , len = strlen(pname);

    // 起点是root目录
    uint32_t parentDirSector = SBlk->root_dir_sector;
    // 记录当前的目录扇区
    uint32_t dir_sector;

    // 路径约定格式，类似于Linux
    // /home/username/123.txt

    // 双指针法解析目录
    for(; idx_end <= len; idx_end++) {
        if(pname[idx_end] == '/' || pname[idx_end] == '\0') {
            if(idx_end - idx_start < 2) {
                // 处理连续的/
                continue;
            }

            memset(dir_name, 0, FS_FILENAME_MAX_LEN + 1);
            // 如果是/a/这种的，start = 0，end = 2，复制从a开始，复制一个字符
            // 因为之前清零了，所以不需要加上\0
            memcpy(dir_name, pname + idx_start + 1, idx_end - idx_start - 1);

            dir_sector = find_directory(dir_name, parentDirSector);

            if(dir_sector == 0) {
                // 创建新目录
                if (create_directory(dir_name, parentDirSector, 0) == false) {
                    return -2; // 创建目录失败
                }
                // 再次查找目录信息
                dir_sector = find_directory(dir_name, parentDirSector);
                if(dir_sector == 0) {
                    // 理论上来说这绝不可能，如果真的出现，说明文件系统坏了
                    return -3;
                }
            }
            // 无论如何都要更换父结点
            parentDirSector = dir_sector;
            // 更新指针
            idx_start = idx_end;
        }
    }

    // 读取目录信息
    DirectoryInfo dir_info;
    if (read_directory(&dir_info, dir_sector) == false) {
        return -1; // 读取目录信息失败
    }

    // 初始化文件描述符
    pdesc->dir_sector_idx = dir_sector;
    pdesc->fmi_sector_idx = dir_info.fileMetadata_sector_idx;
    pdesc->file_pos = 0;

    return 0;
}

// 关闭文件
int16_t fs_close(FS_DESC *pdesc) {
    if(!pdesc || !pdesc->dir_sector_idx) {
        return -1;
    }
    // 因为内容保存工作都被其他操作做了
    // 所以这里只用来更新crc32
    fileCrc32(pdesc->fmi_sector_idx);
    return 0;
}

// 读取文件
int32_t fs_read(FS_DESC *pdesc, uint8_t *pdest, uint32_t anz) {
    if(!pdesc || !pdesc->dir_sector_idx || !pdesc->fmi_sector_idx) {
        return -1;
    }
    if(file_read_context(pdesc->fmi_sector_idx, pdesc->file_pos, pdest, anz) > 0) {
        pdesc->file_pos += anz;
        return 0;
    }
    return -1;
}

// 写入文件
int16_t fs_write(FS_DESC *pdesc, uint8_t *pdata, uint32_t len) {
    if(!pdesc || !pdesc->dir_sector_idx) {
        return -1;
    }
    if(pdesc->fmi_sector_idx == 0) {
        // 建立一个文件元信息
        uint32_t fmi_sector_idx = file_create();
        if(fmi_sector_idx == 0) {
            // 创建文件元信息失败
            return -2;
        }
        // 把目录转变为文件
        if (convert_dir_to_file(pdesc->dir_sector_idx, fmi_sector_idx) == false ) {
            // 转变失败，说明有问题
            // 销毁文件元信息
            file_delete(fmi_sector_idx);
            return -3;
        }
        pdesc->fmi_sector_idx = fmi_sector_idx;
    }

    uint32_t byte_written = file_write_context(pdesc->fmi_sector_idx, pdesc->file_pos, pdata, len);
    if(byte_written > 0) {
        pdesc->file_pos += byte_written;
        return 0;
    }
    return -1;
}

// 重置文件位置
int16_t fs_rewind(FS_DESC *pdesc) {
    if(!pdesc || !pdesc->dir_sector_idx) {
        return -1;
    }

    if(pdesc->file_pos > 0 && pdesc->fmi_sector_idx == 0) {
        // 这是什么情况我没搞明白
        return -1;
    }

    pdesc->file_pos = 0;

    return 0;
}

// 移动文件（理论上来说没目录不行，我把这个改成了可以一路创建了）
int16_t fs_move(FS_DESC *pdesc, char* pname) {
    if(strlen(pname) == 0) {
        return -1;
    }
    if(pname[0] != '/') {
        // 路径必须是/开头的绝对路径
        return -1;
    }
    // 解析路径
    char dir_name[FS_FILENAME_MAX_LEN + 1] = { 0 };
    // 因为第一个字符肯定是/，所以从第二个字符开始扫
    uint32_t idx_start = 0, idx_end = 1 , len = strlen(pname);

    // 起点是root目录
    uint32_t parentDirSector = SBlk->root_dir_sector;
    // 记录当前的目录扇区
    uint32_t dir_sector;

    // 路径约定格式，类似于Linux
    // /home/username/123.txt

    // 双指针法解析目录
    for(; idx_end <= len; idx_end++) {
        if(pname[idx_end] == '/' || pname[idx_end] == '\0') {
            if(idx_end - idx_start < 2) {
                // 处理连续的/
                continue;
            }

            memset(dir_name, 0, FS_FILENAME_MAX_LEN + 1);
            // 如果是/a/这种的，start = 0，end = 2，复制从a开始，复制一个字符
            // 因为之前清零了，所以不需要加上\0
            memcpy(dir_name, pname + idx_start + 1, idx_end - idx_start - 1);

            dir_sector = find_directory(dir_name, parentDirSector);

            if(dir_sector == 0) {
                // 创建新目录
                if (create_directory(dir_name, parentDirSector, 0) == false) {
                    return -2; // 创建目录失败
                }
                // 再次查找目录信息
                dir_sector = find_directory(dir_name, parentDirSector);
                if(dir_sector == 0) {
                    // 理论上来说这绝不可能，如果真的出现，说明文件系统坏了
                    return -3;
                }
            }
            // 无论如何都要更换父结点
            parentDirSector = dir_sector;
            // 更新指针
            idx_start = idx_end;
        }
    }

    // 读取目录信息
    bool res = move_directory(pdesc->dir_sector_idx, dir_sector);

    if(res) {
        return 0;
    }

    return -1;
}

// 删除文件
int16_t fs_delete(FS_DESC *pdesc) {
    if(!pdesc || !pdesc->dir_sector_idx) {
        return -1;
    }
    if(delete_directory(pdesc->dir_sector_idx)) {
        return 0;
    }
    return -1;
}

// 重命名文件
int16_t fs_rename(FS_DESC *pdesc, const char *pname) {
    if(!pdesc || !pdesc->dir_sector_idx) {
        return -1;
    }
    if(rename_directory(pdesc->dir_sector_idx, pname)) {
        return 0;
    }
    return -1;
}

// 获取文件的 CRC32 值
uint32_t fs_get_crc32(FS_DESC *pdesc) {
    if(!pdesc || !pdesc->dir_sector_idx) {
        // 目录不存在的话要返回0xffffffff
        return 0xffffffff;
    }
    if(pdesc->fmi_sector_idx == 0) {
        // 文件不存在的话要返回0xffffffff
        return 0xffffffff;
    }
    read_sector(pdesc->fmi_sector_idx);
    return ((FileMetaInfo *)fs_buffer)->fileCrc32;
}

// 获取文件统计信息
int16_t fs_info(FS_DESC *pdesc,FS_STAT *stat) {
    if(!pdesc || !pdesc->dir_sector_idx) {
        return -1;
    }
    if(!is_valid_directory(pdesc->dir_sector_idx)) {
        // 这是一个无效的目录
        return -1;
    }
    memset(stat, 0, sizeof(FS_STAT));
    read_sector(pdesc->dir_sector_idx);
    strcpy(stat->fname, ((DirectoryInfo *)fs_buffer)->directoryName);
    // 目录的时间戳为0，crc32为0xffffffff
    stat->file_crc32 = 0xffffffff;
    // 如果是文件就接着读
    if(pdesc->fmi_sector_idx) {
        // fmi不是0的情况下可以试试接着读
        if(!is_valid_fileMetaInfo(pdesc->fmi_sector_idx)) {
            // 这是一个有效的目录，但不是一个有效的文件
            return -1;
        }
        read_sector(pdesc->fmi_sector_idx);
        stat->file_len = ((FileMetaInfo *)fs_buffer)->fileLength;
        stat->file_ctime = ((FileMetaInfo *)fs_buffer)->creationTime;
        stat->file_mtime = ((FileMetaInfo *)fs_buffer)->modificationTime;
        stat->file_crc32 = ((FileMetaInfo *)fs_buffer)->fileCrc32;
    }

    return 0;
}


// 为了实现时间戳的工具宏定义、数组、函数

#define SECONDS_PER_MINUTE 60
#define MINUTES_PER_HOUR 60
#define HOURS_PER_DAY 24
#define SECONDS_PER_HOUR (SECONDS_PER_MINUTE * MINUTES_PER_HOUR)
#define SECONDS_PER_DAY (SECONDS_PER_HOUR * HOURS_PER_DAY)
#define DAYS_PER_YEAR 365
#define DAYS_PER_LEAP_YEAR 366

static const uint16_t days_in_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// 判断是否是闰年
int is_leap_year(int year) {
    if (year % 4 == 0) {
        if (year % 100 == 0) {
            if (year % 400 == 0) {
                return 1;
            } else {
                return 0;
            }
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}


// 将时间戳转换为日期
void fs_timestamp_to_date(uint32_t asecs, FS_DATE *pd) {
    uint32_t days = asecs / SECONDS_PER_DAY;
    uint32_t seconds_in_day = asecs % SECONDS_PER_DAY;

    // 计算小时、分钟和秒
    pd->hour = seconds_in_day / SECONDS_PER_HOUR;
    pd->minute = (seconds_in_day % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE;
    pd->second = seconds_in_day % SECONDS_PER_MINUTE;

    uint32_t year = 1970;
    while (1) {
        uint32_t days_in_year = is_leap_year(year) ? DAYS_PER_LEAP_YEAR : DAYS_PER_YEAR;
        if (days >= days_in_year) {
            days -= days_in_year;
            year++;
        } else {
            break;
        }
    }
    pd->year = year;

    uint32_t month = 0;
    while (1) {
        uint32_t days_in_month_current = days_in_month[month];
        if (month == 1 && is_leap_year(year)) {
            days_in_month_current++; // 闰年的二月
        }
        if (days >= days_in_month_current) {
            days -= days_in_month_current;
            month++;
        } else {
            break;
        }
    }
    pd->month = month + 1; // 月份是从1开始
    pd->day = days + 1;    // 日期是从1开始
}

// 将日期转换为时间戳
uint32_t fs_date_to_timestamp(FS_DATE *pd) {
    uint32_t total_days = 0;

    // 计算从1970年到目标年份的总天数
    for (uint32_t year = 1970; year < pd->year; year++) {
        total_days += is_leap_year(year) ? DAYS_PER_LEAP_YEAR : DAYS_PER_YEAR;
    }

    // 计算目标年份中，从1月到目标月份的总天数
    for (uint32_t month = 0; month < pd->month - 1; month++) {
        total_days += days_in_month[month];
        if (month == 1 && is_leap_year(pd->year)) {
            total_days++; // 闰年的二月
        }
    }

    // 加上目标月份中的天数
    total_days += pd->day - 1;

    // 计算总秒数
    uint32_t total_seconds = total_days * SECONDS_PER_DAY;
    total_seconds += pd->hour * SECONDS_PER_HOUR;
    total_seconds += pd->minute * SECONDS_PER_MINUTE;
    total_seconds += pd->second;

    return total_seconds;
}
