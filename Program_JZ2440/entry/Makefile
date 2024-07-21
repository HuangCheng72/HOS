
# 目录
ENTRY_DIR = .

# 源文件
HEAD_ASM = $(ENTRY_DIR)/head.asm
INIT_C_SRC = $(ENTRY_DIR)/init.c
KERNEL_C_SRC = $(ENTRY_DIR)/kernel.c

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra

# 汇编标志
ASM_FLAGS = -o

.PHONY: all

all: head.o init.o kernel.o

# arm交叉编译工具链的gcc没问题了，可以直接编目标文件.o
kernel.o: $(KERNEL_C_SRC)
	$(GCC) $(CFLAGS) -c -o $@ $<

init.o: $(INIT_C_SRC)
	$(GCC) $(CFLAGS) -c -o $@ $<

head.o: $(HEAD_ASM)
	$(AS) $(ASM_FLAGS) $@ $<

# 清理规则
clean:
	@if exist $(ENTRY_DIR)\\*.o del $(ENTRY_DIR)\\*.o
	@if exist $(ENTRY_DIR)\\*.elf del $(ENTRY_DIR)\\*.elf
