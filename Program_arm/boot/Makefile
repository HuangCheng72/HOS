
# 目录
BOOT_DIR = .

# 源文件
START_SRC = $(BOOT_DIR)/start.asm

# 输出文件
START_O = $(BOOT_DIR)/start.o

# 汇编标志
ASM_FLAGS = -o

.PHONY: all

all: $(START_O)

# 编译 start.o
$(START_O): $(START_SRC)
	$(AS) $(ASM_FLAGS) $@ $<

# 清理规则
clean:
	@if exist $(BOOT_DIR)\\*.o del $(BOOT_DIR)\\*.o
