//
// Created by huangcheng on 2024/6/20.
//

#ifndef HOS_FS_H
#define HOS_FS_H


/*******************************************************************************
 * 这是我自己实现的一个简单的文件系统
 * 思路上用的都是最简单的思路，比《操作系统真象还原》的作者实现的还简单
 * 主打一个能用就行
 * 这里是对外暴露的公共读写接口
 *
 * 系统名：HCFileSystem
 *
 * (C) huangcheng
 * Version: 1.0 / 20.06.2024
 *
 ******************************************************************************/

#include "../lib/type.h"

#define DISK_SIZE 16 * 1024 * 1024 // 外存大小（16MB）

// 最大文件名长度
#define FS_FILENAME_MAX_LEN 255  // 最大文件名长度 (第256个字节必须是0)

// 文件格式化选项
#define FS_FORMAT_FULL  1  // 完全格式化 (慢，要清盘)
#define FS_FORMAT_SOFT  2  // 软格式化 (快，只是简单重建超级块和位图)

// 文件打开要控制权限，我懒得写这部分逻辑了，为了方便测试，直接所有文件都是完整权限

// 文件状态标志（这部分没用到，默认全都ACTIVE了）
#define FS_STAT_ACTIVE 1
#define FS_STAT_INACTIVE 2
#define FS_STAT_UNCLOSED 4
#define FS_STAT_INDEX   128   // 诊断用索引之外的请求

// 文件系统状态标志
#define FS_STATE_RUNNING 1   // 正常运行
#define FS_STATE_EXIT 2      // 已经退出

// 由于目录信息和文件元信息做了太多填充，反正已经实现了
// file_read_fileMetaInfo
// file_write_fileMetaInfo
// read_directory
// write_directory
// 只需要保存两个扇区ID，随时都能读入信息和写出信息
// 可以隐藏所有细节

// 文件描述符
typedef struct {
    uint32_t dir_sector_idx;        // 目录所在扇区
    uint32_t fmi_sector_idx;        // 文件元信息所在扇区（其实不保存也无所谓，目录信息里面也有，多读一次而已）
    uint32_t file_pos;              // 操作到文件数据当前位置
} FS_DESC;


// 文件统计信息
typedef struct{
    char fname[FS_FILENAME_MAX_LEN + 1];    // 最大文件名长度
    uint32_t file_ctime;                    // 文件创建时间，自 1970年1月1日 0:00:00 以来的秒数，即时间戳
    uint32_t file_mtime;                    // 文件上次修改时间，自 1970年1月1日 0:00:00 以来的秒数，即时间戳
    uint32_t file_len;                      // 文件长度
    uint32_t file_crc32;                    // 此文件的 CRC32，按照 ISO 3309 计算，未使用时为 0xffffffff
} FS_STAT;

// 时间结构体
typedef struct{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;  // 年份，1970 - 2099 (2100 不是闰年)
} FS_DATE;

//-------------------- 文件系统函数接口 --------------------------
// 文件系统启动
int16_t fs_start(void);
// 文件系统退出
int16_t fs_exit(void);
// 格式化文件系统
int16_t fs_format(uint8_t fmode);
// 打开文件
int16_t fs_open(FS_DESC *pdesc, char* pname);
// 关闭文件
int16_t fs_close(FS_DESC *pdesc);
// 读取文件
int32_t fs_read(FS_DESC *pdesc, uint8_t *pdest, uint32_t anz);
// 写入文件
int16_t fs_write(FS_DESC *pdesc, uint8_t *pdata, uint32_t len);
// 移动文件
int16_t fs_move(FS_DESC *pdesc, char* pname);
// 删除文件
int16_t fs_delete(FS_DESC *pdesc);
// 重命名文件
int16_t fs_rename(FS_DESC *pdesc, const char *pname);
// 重置文件位置
int16_t fs_rewind(FS_DESC *pdesc);
// 获取文件的 CRC32 值
uint32_t fs_get_crc32(FS_DESC *pdesc);
// 获取文件统计信息
int16_t fs_info(FS_DESC *pdesc,FS_STAT *stat);
// 将时间戳转换为日期
void fs_timestamp_to_date(uint32_t asecs, FS_DATE *pd);
// 将日期转换为时间戳
uint32_t fs_date_to_timestamp(FS_DATE *pd);

#endif //HOS_FS_H
