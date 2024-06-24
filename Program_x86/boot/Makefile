
# 目录
BOOT_DIR = .
INCLUDE_DIR = $(BOOT_DIR)/include

# 源文件
MBR_SRC = $(BOOT_DIR)/mbr.asm
LOADER_SRC = $(BOOT_DIR)/loader.asm

# 输出文件
MBR_BIN = $(BOOT_DIR)/mbr.bin
LOADER_BIN = $(BOOT_DIR)/loader.bin

# 汇编标志
ASM_FLAGS = -f bin -I $(INCLUDE_DIR)/

.PHONY: all

all: $(MBR_BIN) $(LOADER_BIN)

# 编译 mbr.bin
$(MBR_BIN): $(MBR_SRC) $(INCLUDE_DIR)/boot.inc
	$(NASM) $(ASM_FLAGS) -o $@ $<

# 编译 loader.bin
$(LOADER_BIN): $(LOADER_SRC) $(INCLUDE_DIR)/boot.inc
	$(NASM) $(ASM_FLAGS) -o $@ $<

# 清理规则
clean:
	@if exist $(BOOT_DIR)\\*.bin del $(BOOT_DIR)\\*.bin
