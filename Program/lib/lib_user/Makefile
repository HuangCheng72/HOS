# 目录
LIB_DIR = ..
LIB_USER = .

INTR_SYSCALL_ASM_SRC = $(LIB_USER)/intr_syscall.asm
SYSCALL_C_SRC = $(LIB_USER)/syscall.c

# 汇编标志
ASM_FLAGS = -f elf32

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra

# 默认目标，编译所有的 .asm 文件
all: $(LIB_DIR)/intr_syscall.o $(LIB_DIR)/syscall.o

# 目标文件生成规则

$(LIB_DIR)/intr_syscall.o : $(INTR_SYSCALL_ASM_SRC)
	$(NASM) $(ASM_FLAGS) -o $@ $<

# C 文件生成 汇编文件，再汇编生成.o，为什么这么做之前说过了，这个gcc的汇编器不知道什么问题
# 编出来放到模块文件夹kernel去，别放在这里，方便主Makefile找
$(LIB_DIR)/syscall.o: $(SYSCALL_C_SRC)
	$(GCC) $(CFLAGS) -S -o $(LIB_USER)/syscall.asm $<
	$(AS) -o $@ $(LIB_USER)/syscall.asm
	del $(LIB_USER)\\syscall.asm

# 文件不在这里生成，无需清理
