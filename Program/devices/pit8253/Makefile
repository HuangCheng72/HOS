
# 目录
DEVICE_DIR = ..
PIT8253_DIR = .

# 源文件
PIT8253_C_SRC = $(PIT8253_DIR)/pit8253.c

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra

# 标签标记所有的任务（输出什么文件），方便修改
DEVICE_OBJS = $(DEVICE_DIR)/pit8253.o

.PHONY: all

all: $(DEVICE_OBJS)

# C 文件生成 汇编文件，再汇编生成.o，为什么这么做之前说过了，这个gcc的汇编器不知道什么问题
# 编出来放到模块文件夹devices去，别放在这里，方便主Makefile找
$(DEVICE_DIR)/pit8253.o: $(PIT8253_C_SRC)
	$(GCC) $(CFLAGS) -S -o $(PIT8253_DIR)/pit8253.asm $<
	$(AS) -o $@ $(PIT8253_DIR)/pit8253.asm
	del $(PIT8253_DIR)\\pit8253.asm

# 文件不生成于此，无需清理规则
