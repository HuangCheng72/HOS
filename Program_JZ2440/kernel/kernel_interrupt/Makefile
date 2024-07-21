
# 目录
KERNEL_DIR = ..
KERNEL_INTERRUPT_DIR = .

# 源文件
KERNEL_INTERRUPT_C_SRC = $(KERNEL_INTERRUPT_DIR)/kernel_interrupt.c
CLISTI_ASM = $(KERNEL_INTERRUPT_DIR)/clisti.asm

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra

# 汇编标志
ASM_FLAGS = -o

# 标签标记所有的任务（输出什么文件），方便修改
KERNEL_OBJS = $(KERNEL_DIR)/kernel_interrupt.o $(KERNEL_DIR)/clisti.o

.PHONY: all

all: $(KERNEL_OBJS)

# 编出来放到模块文件夹kernel去，别放在这里，方便主Makefile找
$(KERNEL_DIR)/kernel_interrupt.o: $(KERNEL_INTERRUPT_C_SRC)
	$(GCC) $(CFLAGS) -c -o $@ $<

$(KERNEL_DIR)/clisti.o: $(CLISTI_ASM)
	$(AS) $(ASM_FLAGS) $@ $<

# 文件不生成于此，无需清理规则
