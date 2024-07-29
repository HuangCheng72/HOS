
# 目录
KERNEL_DIR = ..
KERNEL_BUFFER_DIR = .

# 源文件
KERNEL_BUFFER_C_SRC = $(KERNEL_BUFFER_DIR)/kernel_buffer.c

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra

# 标签标记所有的任务（输出什么文件），方便修改
KERNEL_OBJS = $(KERNEL_DIR)/kernel_buffer.o

.PHONY: all

all: $(KERNEL_OBJS)

# 编出来放到模块文件夹kernel去，别放在这里，方便主Makefile找
$(KERNEL_DIR)/kernel_buffer.o: $(KERNEL_BUFFER_C_SRC)
	$(GCC) $(CFLAGS) -c -o $@ $<

# 文件不生成于此，无需清理规则
