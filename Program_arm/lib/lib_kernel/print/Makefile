
# 目录
LIB_DIR = ../..
LIB_KERNEL_DIR = ..
PRINT_DIR = .

# 源文件
PRINT_C_SRC = $(PRINT_DIR)/print.c

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra

# 标签标记所有的任务（输出什么文件），方便修改
OBJS = $(LIB_DIR)/print.o

.PHONY: all

all: $(OBJS)

# 编出来放到模块文件夹kernel去，别放在这里，方便主Makefile找
$(LIB_DIR)/print.o: $(PRINT_C_SRC)
	$(GCC) $(CFLAGS) -c -o $@ $<

# 文件不生成于此，无需清理规则
