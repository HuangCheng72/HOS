
# 目录
KERNEL_DIR = ..
KERNEL_TASK_DIR = .

# 源文件
KERNEL_TASK_C_SRC = $(KERNEL_TASK_DIR)/kernel_task.c

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra

# 汇编标志
ASM_FLAGS = -f elf32

# 标签标记所有的任务（输出什么文件），方便修改
OBJS = $(KERNEL_DIR)/kernel_task.o

.PHONY: all

all: $(OBJS)

# 编出来放到模块文件夹kernel去，别放在这里，方便主Makefile找
$(KERNEL_DIR)/kernel_task.o: $(KERNEL_TASK_C_SRC)
	$(GCC) $(CFLAGS) -c -o $@ $<

# 不生成文件于此，无需清理
