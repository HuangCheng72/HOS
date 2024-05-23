# Makefile 用于编译项目

# 汇编器和编译器、链接器等各种工具
NASM = nasm
GCC = i386-elf-gcc
AS = i386-elf-as
LD = i386-elf-ld

# 目录
BOOT_DIR = boot
KERNEL_DIR = kernel

# 源文件
MBR_SRC = $(BOOT_DIR)/mbr.asm
LOADER_SRC = $(BOOT_DIR)/loader.asm
KERNEL_FUNC_ASM_SRC = $(KERNEL_DIR)/kernel_func.asm
KERNEL_C_SRC = $(KERNEL_DIR)/kernel.c
KERNEL_LINKER_SCRIPT = $(KERNEL_DIR)/kernel_linker.ld

# 头文件
BOOT_INCLUDE_DIR = $(BOOT_DIR)/include
TYPE_H_SRC = $(KERNEL_DIR)/type.h

# 输出文件
MBR_BIN = $(BOOT_DIR)/mbr.bin
LOADER_BIN = $(BOOT_DIR)/loader.bin
KERNEL_BIN = $(KERNEL_DIR)/kernel.bin

# 汇编标志
BOOT_ASM_FLAGS = -f bin -I $(BOOT_INCLUDE_DIR)/
KERNEL_ASM_FLAGS = -f elf32

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra

# 链接标志
KERNEL_LDFLAGS = -T $(KERNEL_LINKER_SCRIPT) --oformat binary

.PHONY: all

all: $(MBR_BIN) $(LOADER_BIN) $(KERNEL_BIN)

# 编译 mbr.bin
$(MBR_BIN): $(MBR_SRC) $(BOOT_INCLUDE_DIR)/boot.inc
	$(NASM) $(BOOT_ASM_FLAGS) -o $@ $<

# 编译 loader.bin
$(LOADER_BIN): $(LOADER_SRC) $(BOOT_INCLUDE_DIR)/boot.inc
	$(NASM) $(BOOT_ASM_FLAGS) -o $@ $<

# 生成 kernel.bin
$(KERNEL_BIN): $(KERNEL_DIR)/kernel.o $(KERNEL_DIR)/kernel_func.o
	$(LD) $(KERNEL_LDFLAGS) -o $@ $^

# 汇编文件生成 .o 文件
$(KERNEL_DIR)/kernel_func.o: $(KERNEL_FUNC_ASM_SRC)
	$(NASM) $(KERNEL_ASM_FLAGS) -o $@ $<

# C 文件生成 汇编文件，再汇编生成.o
# 这个GCC不知道用的什么汇编器居然解析不了它自己生成的代码
# 只能另用作者提供的配套的汇编器编
# 而且用GCC解析的汇编代码，只能用AS编译，NASM编译不了
$(KERNEL_DIR)/kernel.o: $(KERNEL_C_SRC) $(TYPE_H_SRC)
	$(GCC) $(CFLAGS) -S -o $(KERNEL_DIR)/kernel.asm $<
	$(AS) -o $@ $(KERNEL_DIR)/kernel.asm
	del $(KERNEL_DIR)\\kernel.asm


# 清理规则
clean:
	@if exist $(BOOT_DIR)\\*.bin del $(BOOT_DIR)\\*.bin
	@if exist $(KERNEL_DIR)\\*.o del $(KERNEL_DIR)\\*.o
	@if exist $(KERNEL_DIR)\\*.bin del $(KERNEL_DIR)\\*.bin
