
# 目录
LIB_DIR = ../..
LIB_KERNEL_DIR = ..
BITMAP_DIR = .

# 源文件
BITMAP_C_SRC = $(BITMAP_DIR)/bitmap.c

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra

# 标签标记所有的任务（输出什么文件），方便修改
OBJS = $(LIB_DIR)/bitmap.o

.PHONY: all

all: $(OBJS)

# C 文件生成 汇编文件，再汇编生成.o，为什么这么做之前说过了，这个gcc的汇编器不知道什么问题
# 编出来放到模块文件夹kernel去，别放在这里，方便主Makefile找
$(LIB_DIR)/bitmap.o: $(BITMAP_C_SRC)
	$(GCC) $(CFLAGS) -S -o $(BITMAP_DIR)/bitmap.asm $<
	$(AS) -o $@ $(BITMAP_DIR)/bitmap.asm
	del $(BITMAP_DIR)\\bitmap.asm

# 文件不生成于此，无需清理规则
