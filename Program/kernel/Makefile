
# 目录
KERNEL_DIR = .

# 源文件
KERNEL_FUNC_ASM_SRC = $(KERNEL_DIR)/kernel_func.asm
KERNEL_C_SRC = $(KERNEL_DIR)/kernel.c

# 头文件
TYPE_H_SRC = $(KERNEL_DIR)/type.h

# 汇编标志
ASM_FLAGS = -f elf32

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra

# 标签标记所有的任务（输出什么文件），方便修改
KERNEL_OBJS = $(KERNEL_DIR)/kernel_func.o $(KERNEL_DIR)/kernel.o

.PHONY: all

all: $(KERNEL_OBJS)

# 汇编文件生成 .o 文件
$(KERNEL_DIR)/kernel_func.o: $(KERNEL_FUNC_ASM_SRC)
	$(NASM) $(ASM_FLAGS) -o $@ $<

# C 文件生成 汇编文件，再汇编生成.o，为什么这么做之前说过了，这个gcc的汇编器不知道什么问题
$(KERNEL_DIR)/kernel.o: $(KERNEL_C_SRC) $(TYPE_H_SRC)
	$(GCC) $(CFLAGS) -S -o $(KERNEL_DIR)/kernel.asm $<
	$(AS) -o $@ $(KERNEL_DIR)/kernel.asm
	del $(KERNEL_DIR)\\kernel.asm

# 清理规则
clean:
	@if exist $(KERNEL_DIR)\\*.o del $(KERNEL_DIR)\\*.o
	@if exist $(KERNEL_DIR)\\*.bin del $(KERNEL_DIR)\\*.bin
