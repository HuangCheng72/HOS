# 目录
INTERRUPT_HANDLER_DIR = .

# 汇编标志
ASM_FLAGS = -f elf32

# 定义源文件和目标文件
SRCS = $(wildcard *.asm)
OBJS = $(SRCS:.asm=.o)

# 默认目标，编译所有的 .asm 文件
all: $(OBJS)

# 目标文件生成规则
%.o: %.asm
	$(NASM) $(ASM_FLAGS) -o $@ $<

# 清理目标，删除所有 .o 文件
clean:
	@if exist $(INTERRUPT_HANDLER_DIR)\\*.o del $(INTERRUPT_HANDLER_DIR)\\*.o
