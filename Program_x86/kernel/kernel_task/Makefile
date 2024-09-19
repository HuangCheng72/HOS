
# 目录
KERNEL_DIR = ..
KERNEL_TASK_DIR = .

# 源文件
KERNEL_TASK_C_SRC = $(KERNEL_TASK_DIR)/kernel_task.c

KERNEL_TASK_SWITCH_TO_ASM = $(KERNEL_TASK_DIR)/switch_to.asm

KERNEL_SYNC_C_SRC = $(KERNEL_TASK_DIR)/kernel_sync.c

KERNEL_TASK_ATOMIC_ASM = $(KERNEL_TASK_DIR)/atomic.asm

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra

# 汇编标志
ASM_FLAGS = -f elf32

# 标签标记所有的任务（输出什么文件），方便修改
OBJS = $(KERNEL_DIR)/kernel_task.o $(KERNEL_DIR)/switch_to.o $(KERNEL_DIR)/kernel_sync.o $(KERNEL_DIR)/atomic.o

.PHONY: all

all: $(OBJS)

# C 文件生成 汇编文件，再汇编生成.o，为什么这么做之前说过了，这个gcc的汇编器不知道什么问题
# 编出来放到模块文件夹kernel去，别放在这里，方便主Makefile找
$(KERNEL_DIR)/kernel_task.o: $(KERNEL_TASK_C_SRC)
	$(GCC) $(CFLAGS) -S -o $(KERNEL_TASK_DIR)/kernel_task.asm $<
	$(AS) -o $@ $(KERNEL_TASK_DIR)/kernel_task.asm
	del $(KERNEL_TASK_DIR)\\kernel_task.asm

$(KERNEL_DIR)/kernel_sync.o: $(KERNEL_SYNC_C_SRC)
	$(GCC) $(CFLAGS) -S -o $(KERNEL_TASK_DIR)/kernel_sync.asm $<
	$(AS) -o $@ $(KERNEL_TASK_DIR)/kernel_sync.asm
	del $(KERNEL_TASK_DIR)\\kernel_sync.asm

$(KERNEL_DIR)/switch_to.o: $(KERNEL_TASK_SWITCH_TO_ASM)
	$(NASM) $(ASM_FLAGS) -o $@ $<

$(KERNEL_DIR)/atomic.o: $(KERNEL_TASK_ATOMIC_ASM)
	$(NASM) $(ASM_FLAGS) -o $@ $<

# 不生成文件于此，无需清理
