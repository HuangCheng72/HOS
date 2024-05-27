
# 目录
KERNEL_DIR = ..
KERNEL_INTERRUPT_DIR = .

LIB_DIR = ../../lib

# 源文件
KERNEL_INTERRUPT_C_SRC = $(KERNEL_INTERRUPT_DIR)/kernel_interrupt.c

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra -I$(LIB_DIR)

# 标签标记所有的任务（输出什么文件），方便修改
KERNEL_OBJS = $(KERNEL_DIR)/kernel_interrupt.o

.PHONY: all

all: $(KERNEL_OBJS)

# C 文件生成 汇编文件，再汇编生成.o，为什么这么做之前说过了，这个gcc的汇编器不知道什么问题
$(KERNEL_DIR)/kernel_interrupt.o: $(KERNEL_INTERRUPT_C_SRC)
	$(GCC) $(CFLAGS) -S -o $(KERNEL_INTERRUPT_DIR)/kernel_interrupt.asm $<
	$(AS) -o $@ $(KERNEL_INTERRUPT_DIR)/kernel_interrupt.asm
	del $(KERNEL_INTERRUPT_DIR)\\kernel_interrupt.asm

# 文件不生成于此，无需清理规则
