
# 目录
KERNEL_DIR = ..
KERNEL_INTERRUPT_DIR = .
#LIB_DIR = ../../lib

# 中断处理函数目录
KERNEL_INTERRUPT_HANDLER_DIR = interrupt_handler

# 源文件
#KERNEL_DEVICE_C_SRC = $(KERNEL_DEVICE_DIR)/kernel_device.c

# C 编译标志
#CFLAGS = -ffreestanding -nostdlib -Wall -Wextra -I$(LIB_DIR)

# 标签标记所有的任务（输出什么文件），方便修改
KERNEL_OBJS = $(KERNEL_DIR)/handler.o

.PHONY: all

all: $(KERNEL_OBJS)

# 执行各子模块编译
$(KERNEL_DIR)/handler.o:
	$(MAKE) -C $(KERNEL_INTERRUPT_HANDLER_DIR) NASM=$(NASM)

# 文件不生成于此，无需清理规则
