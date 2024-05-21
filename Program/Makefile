# Makefile 用于编译项目

# 汇编器和编译器
NASM = nasm

# 路径
BOOT_DIR = boot

# 文件
MBR_SRC = $(BOOT_DIR)/mbr.asm

# 输出文件
MBR_BIN = $(BOOT_DIR)/mbr.bin

# 汇编标志
ASM_FLAGS = -f bin

.PHONY: all

all: $(MBR_BIN)

# 编译 mbr.bin
$(MBR_BIN): $(MBR_SRC)
	$(NASM) $(ASM_FLAGS) -o $@ $<

# 清理规则
clean:
	@if exist $(BOOT_DIR)\\*.bin del $(BOOT_DIR)\\*.bin
