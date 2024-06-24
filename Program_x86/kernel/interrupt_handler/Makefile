# 目录
KERNEL_DIR = ..
INTERRUPT_HANDLER = .

# 源文件
SYSCALL_DISPATCHER_C_SRC = $(INTERRUPT_HANDLER)/syscall_dispatcher.c

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra

# 汇编标志
ASM_FLAGS = -f elf32

# 定义源文件和目标文件
SRCS = $(wildcard *.asm)
OBJS = $(SRCS:.asm=.o)

# 默认目标，编译所有的 .asm 文件
all: $(OBJS) $(KERNEL_DIR)/syscall_dispatcher.o

# 目标文件生成规则
%.o: %.asm
	$(NASM) $(ASM_FLAGS) -o $(KERNEL_DIR)/$@ $<

# C 文件生成 汇编文件，再汇编生成.o，为什么这么做之前说过了，这个gcc的汇编器不知道什么问题
# 编出来放到模块文件夹kernel去，别放在这里，方便主Makefile找
$(KERNEL_DIR)/syscall_dispatcher.o: $(SYSCALL_DISPATCHER_C_SRC)
	$(GCC) $(CFLAGS) -S -o $(INTERRUPT_HANDLER)/syscall_dispatcher.asm $<
	$(AS) -o $@ $(INTERRUPT_HANDLER)/syscall_dispatcher.asm
	del $(INTERRUPT_HANDLER)\\syscall_dispatcher.asm

# 不在此处生成文件无需清理
