
# 目录
KERNEL_DIR = ..
KERNEL_GDT_DIR = .

# 源文件
KERNEL_GDT_C_SRC = $(KERNEL_GDT_DIR)/kernel_gdt.c

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra

# 标签标记所有的任务（输出什么文件），方便修改
KERNEL_OBJS = $(KERNEL_DIR)/kernel_gdt.o

.PHONY: all

all: $(KERNEL_OBJS)

# C 文件生成 汇编文件，再汇编生成.o，为什么这么做之前说过了，这个gcc的汇编器不知道什么问题
# 编出来放到模块文件夹kernel去，别放在这里，方便主Makefile找
$(KERNEL_DIR)/kernel_gdt.o: $(KERNEL_GDT_C_SRC)
	$(GCC) $(CFLAGS) -S -o $(KERNEL_GDT_DIR)/kernel_gdt.asm $<
	$(AS) -o $@ $(KERNEL_GDT_DIR)/kernel_gdt.asm
	del $(KERNEL_GDT_DIR)\\kernel_gdt.asm

# 文件不生成于此，无需清理规则
