
# 目录
USER_DIR = ..
USER_PROCESS_DIR = .

# 源文件
USER_PROCESS_C_SRC = $(USER_PROCESS_DIR)/process.c

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra

# 标签标记所有的任务（输出什么文件），方便修改
USER_OBJS = $(USER_DIR)/process.o

.PHONY: all

all: $(USER_OBJS)

# C 文件生成 汇编文件，再汇编生成.o，为什么这么做之前说过了，这个gcc的汇编器不知道什么问题
# 编出来放到模块文件夹user去，别放在这里，方便主Makefile找
$(USER_DIR)/process.o: $(USER_PROCESS_C_SRC)
	$(GCC) $(CFLAGS) -S -o $(USER_PROCESS_DIR)/process.asm $<
	$(AS) -o $@ $(USER_PROCESS_DIR)/process.asm
	del $(USER_PROCESS_DIR)\\process.asm

# 文件不生成于此，无需清理规则
