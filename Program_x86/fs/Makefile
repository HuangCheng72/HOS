# 目录
FS_DIR = .

# 获取所有的.c文件
FS_C_SRC = $(wildcard $(FS_DIR)/*.c)

# 获取对应的.o文件
FS_OBJS = $(FS_C_SRC:.c=.o)

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra

.PHONY: all

all: $(FS_OBJS)

# C 文件生成 汇编文件，再汇编生成.o
%.o: %.c
	$(GCC) $(CFLAGS) -S -o $(FS_DIR)/$*.asm $<
	$(AS) -o $(FS_DIR)/$*.o $(FS_DIR)/$*.asm
	del $(FS_DIR)\\$*.asm

# 清理规则
clean:
	@if exist $(FS_DIR)\\*.asm del $(FS_DIR)\\*.asm
	@if exist $(FS_DIR)\\*.o del $(FS_DIR)\\*.o
