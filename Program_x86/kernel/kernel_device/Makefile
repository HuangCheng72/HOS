
# 目录
KERNEL_DIR = ..
KERNEL_DEVICE_DIR = .

# 源文件
KERNEL_DEVICE_C_SRC = $(KERNEL_DEVICE_DIR)/kernel_device.c

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra

# 标签标记所有的任务（输出什么文件），方便修改
KERNEL_OBJS = $(KERNEL_DIR)/kernel_device.o

.PHONY: all

all: $(KERNEL_OBJS)

# C 文件生成 汇编文件，再汇编生成.o，为什么这么做之前说过了，这个gcc的汇编器不知道什么问题
# 编出来放到模块文件夹kernel去，别放在这里，方便主Makefile找
$(KERNEL_DIR)/kernel_device.o: $(KERNEL_DEVICE_C_SRC)
	$(GCC) $(CFLAGS) -S -o $(KERNEL_DEVICE_DIR)/kernel_device.asm $<
	$(AS) -o $@ $(KERNEL_DEVICE_DIR)/kernel_device.asm
	del $(KERNEL_DEVICE_DIR)\\kernel_device.asm

# 文件不生成于此，无需清理规则
