# 目录
KERNEL_DIR = ../..
KERNEL_INTERRUPT_HANDLER_DIR = .

# 汇编标志
ASM_FLAGS = -f elf32

# 定义源文件和目标文件
SRCS = $(wildcard *.asm)
OBJS = $(SRCS:.asm=.o)

# 默认目标，编译所有的 .asm 文件
all: $(OBJS)

# 目标文件生成规则
%.o: %.asm
	$(NASM) $(ASM_FLAGS) -o $(KERNEL_DIR)/$@ $<

# 不生成文件于此，无需清理
