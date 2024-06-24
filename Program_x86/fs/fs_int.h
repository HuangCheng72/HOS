//
// Created by huangcheng on 2024/6/20.
//

#ifndef HOS_FS_INT_H
#define HOS_FS_INT_H

// 这里是文件系统的内部所有的接口，负责用来支持实现fs给出的公共接口

#include "../lib/lib_kernel/lib_kernel.h"

// HCFileSystem的基本设计思想：
// 以扇区（512字节）为基本单位，定死
// 外存开头扇区放这个文件系统的超级块（凑够512个字节）
// 前2MB是放超级块和目录信息的。
// 超级块存放外存的位图信息
// 为了方便管理，目录信息也用512字节（这样一个目录对应一个完整扇区）
// 目录信息之间互相组织用的是类似于链表的方法，以扇区号代替指针（在外存里面用指针感觉不太可靠）
// 因为一个目录对应一个完整扇区，所以扇区号和目录能够一一对应，形成唯一性
// 目录信息定义的是文件层级信息
// 2MB开始是文件元信息（真正的文件数据所在）

// 超级块结构体，读入后要长期驻留在内存中
typedef struct {
    char magic_header[16];              // 文件系统标识符，"HCFileSystemMag"
    uint32_t total_sectors;             // 外存总扇区数（我不管你多大，我只用扇区数计算）
    uint32_t bitmap_sectors;            // 位图占用的扇区数（位图紧跟在超级块后面）
    uint32_t root_dir_sector;           // 根目录所在的扇区号
    uint32_t file_meta_start_sector;    // 文件元信息区域起始扇区
    uint32_t free_sectors;              // 剩余可用扇区数
    char magic_footer[16];              // 超级块有效标识符，"SuperBlockValid"
    // 因为超级块要长期驻留在内存中，所以可以把位图管理信息写在这里
    BitMap btmp;
    // 以上一共68字节
    char reserved[452];                 // 填充到512字节
} SuperBlock;

// 目录信息
typedef struct {
    char magic_header[16];      // 统一为"HCFileSystemDir"
    char directoryName[256];    // 目录名
    uint32_t sector_idx;        // 该目录所在的扇区索引
    uint32_t parentDirectory_sector_idx;        // 父目录所在的扇区索引
    uint32_t prevSiblingDirectory_sector_idx;   // 前一个兄弟目录所在的扇区索引
    uint32_t nextSiblingDirectory_sector_idx;   // 后一个兄弟目录所在的扇区索引
    uint32_t subdirectory_sector_idx;           // 任意一个子目录所在的扇区索引，如果是一个文件，这一项必须为0
    uint32_t subdirectory_count;                // 子目录计数器
    uint32_t fileMetadata_sector_idx;           // 如果是一个文件，这一项不得为0
    char magic_header2[16];                     // 统一为"ThisDirIsValid_"
    uint32_t checksum;                          // 校验和
    char fill[196];             // 填充到512字节
} DirectoryInfo;

// 文件元信息
typedef struct {
    char magic_header[16];          // 魔数，用于识别文件元信息，统一为"HCFSFileMetaMgc"
    uint32_t sector_idx;            // 所在扇区索引
    uint32_t fileLength;            // 文件长度（方便管理，不包括文件元信息）
    uint32_t sector_count;          // 文件包括文件元信息总共占据了多少个扇区（纯粹是我懒，懒得再计算直接写进去）
    uint32_t creationTime;          // 创建时间戳
    uint32_t modificationTime;      // 修改时间戳
    uint32_t directoryCounter;      // 目录计数器（多少个目录指向文件元信息，这是为了解决复制的问题）
    uint32_t fileCrc32;             // 文件信息的crc32，用于校验
    uint32_t next_file_sector_idx;  // 下一个文件扇区

    // 为了偷懒，文件元信息也占据512个字节，凑足一个扇区
    char fill[454];

    // 文件内容紧随文件元信息之后
} FileMetaInfo ;

// 文件信息（正好是一个扇区大小）
typedef struct {
    uint32_t magic_number;          // 字符是：“fis1”，写成16进制4字节数就是：0x66697331
    uint32_t sector_idx;            // 所在扇区索引
    uint32_t next_file_sector_idx;  // 下一个文件扇区
    uint8_t data[500];              // 具体的文件数据
} FileInfo;

// 长期驻留在内存中的超级块（驻留位置固定，在ml中初始化）
extern SuperBlock *SBlk;
// 因为以512个字节为基本操作单位，所以文件系统缓冲区大小设计为512个字节（位置固定，大小固定，，在ml中初始化）
extern uint8_t *fs_buffer;

// 操作系统必须已经支持的操作
extern uint32_t get_unix_timestamp_now(void);  // 取现行时间戳

/************************************ 高级层（直接支持对外暴露的文件读写接口的实现）************************************/

// 超级块操作

// 判断超级块是否有效
bool is_valid_superblock();
// 初始化超级块，参数是外存一共有多少个扇区
void init_superblock(uint32_t total_sectors);
// 从外存读入超级块，并恢复管理位图
void read_superblock();
// 把超级块和管理信息写入到外存
void write_superblock();

// 目录操作

// 判断这个扇区存放的目录是否有效
bool is_valid_directory(uint32_t sector_idx);
// 查找目录（查找起点目录包括所有子目录，因此要递归查找），得到结果是符合要求的目录索引
uint32_t find_directory(const char* name, uint32_t sector_idx);
// 只查给定目录的子目录这一层级，不递归，得到结果是符合要求的目录索引
uint32_t find_subdirectory(const char* name, uint32_t sector_idx);
// 创建根目录，只能使用一次（在初始化文件系统的时候使用）
bool create_root_directory();
// 创建目录（如果是一个文件的话，file_metadata_sector_idx不得为0），返回操作是否成功
bool create_directory(const char *name, uint32_t parent_sector_idx, uint32_t file_metadata_sector_idx);
// 删除这个目录（连带子目录），返回操作是否成功
bool delete_directory(uint32_t sector_idx);
// 移动该目录到目标目录下，成为目标目录的子目录，目标目录是新目录的父目录，返回操作是否成功
bool move_directory(uint32_t src_sector_idx, uint32_t dest_sector_idx);
// 复制该目录连带其子目录到目标目录下，该目录成为目标目录下的子目录，目标目录是新目录的父目录，返回操作是否成功
bool copy_directory(uint32_t src_sector_idx, uint32_t dest_sector_idx);
// 重命名目录，返回操作是否成功
bool rename_directory(uint32_t sector_idx, const char* new_name);
// 读入目录信息到给定的存储目录信息的空间，参数是存储目录信息和目录信息所在扇区索引，返回值是是否读出一个有效的目录信息
bool read_directory(DirectoryInfo *dir_buffer, uint32_t sector_idx);
// 写出给定的目录信息到指定的扇区，参数是存储目录信息的空间和目录信息所在扇区索引，返回值是是否写出成功
bool write_directory(DirectoryInfo *dir_buffer, uint32_t sector_idx);

// 文件元信息操作

// 校验这个扇区是否是一个有效的文件元信息
bool is_valid_fileMetaInfo(uint32_t sector_idx);
// 读入文件元信息到给定的存储文件元信息的空间，参数是存储文件元信息的空间和文件元信息所在扇区索引，返回值是是否读出一个有效的文件元信息
bool file_read_fileMetaInfo(FileMetaInfo *fmi_buffer, uint32_t fileMetaSector_idx);
// 写出给定的文件元信息到指定的扇区，参数是存储文件元信息的空间和文件元信息所在扇区索引，返回值是是否写出成功
bool file_write_fileMetaInfo(FileMetaInfo *fmi_buffer, uint32_t fileMetaSector_idx);
// 常用操作：文件元信息目录计数器+1，传入参数是文件元信息所在的扇区索引，返回是操作是否成功
bool fileMetaInfo_directoryCounter_Increment(uint32_t fileMetaSector_idx);
// 常用操作：文件元信息目录计数器-1，传入参数是文件元信息所在的扇区索引，返回是操作是否成功
bool fileMetaInfo_directoryCounter_Decrement(uint32_t fileMetaSector_idx);

// 文件操作

// 校验这个扇区开头是否是一个有效的文件信息
bool is_valid_fileInfo(uint32_t sector_idx);
// 文件创建，创建一个空白的文件元信息，返回文件元信息所在的扇区
uint32_t file_create();
// 文件删除，返回是否成功，参数是文件元信息所在扇区索引
bool file_delete(uint32_t fileMetaSector_idx);
// 读入文件数据到给定的buffer，参数是文件元信息所在扇区索引，开始读取位置，buffer，读取长度，返回值是读出数据的长度（以字节计算）
uint32_t file_read_context(uint32_t fileMetaSector_idx, uint32_t pos, void *buffer, uint32_t len);
// 将给定的数据写出到文件某个位置之后，参数是文件元信息所在扇区索引，开始写出位置（注意，该位置之后的数据将会永久丢失），buffer，读取长度，返回值是读出数据的长度（以字节计算）
uint32_t file_write_context(uint32_t fileMetaSector_idx, uint32_t pos, void *buffer, uint32_t len);

// 文件和目录之间的操作

// 将目录转变为文件，该目录不能有子目录，否则会被认为是目录而非文件（其实就是修改，把文件元信息扇区加上去而已）
bool convert_dir_to_file(uint32_t dir_sector_idx, uint32_t fileMetaSector_idx);

// 修改的话就是删除又重新创建（新增一个版本）

// 额外的工具函数

// 计算文件的crc32，参数是文件元信息扇区（直接更新写入外存）
void fileCrc32(uint32_t fileMetaSector_idx);

/************************************ 高级层（直接支持对外暴露的文件读写接口的实现）************************************/


/************************************ 中级层（支持高级层的实现，是硬件抽象层）************************************/

// 读入一个扇区数据到文件系统缓冲区（读入前清空缓冲区）
void read_sector(uint32_t sector_idx);
// 把文件系统缓冲区的信息写出到扇区（写出后清空缓冲区）
void write_sector(uint32_t sector_idx);

/************************************ 中级层（支持高级层的实现，是硬件抽象层）************************************/

/************************************ 低级层（支持中级层的实现，是硬件驱动层）************************************/

// 我这个文件系统不需要低级层，因为是建立在ramdisk之上，实际上低级层就是ramdisk驱动，早就写过了

/************************************ 低级层（支持高级层的实现，是硬件驱动层）************************************/

#endif //HOS_FS_INT_H
