# 目录
LIB_DIR = ..
LIB_KERNEL = .
STRING_DIR = string
BITMAP_DIR = bitmap
LIST_DIR = list

# 汇编标志
ASM_FLAGS = -f elf32

# 定义源文件和目标文件
SRCS = $(wildcard *.asm)
OBJS = $(SRCS:.asm=.o)

# 默认目标，编译所有的 .asm 文件
all: $(OBJS) $(LIB_DIR)/string.o $(LIB_DIR)/bitmap.o $(LIB_DIR)/list.o

# 目标文件生成规则
%.o: %.asm
	$(NASM) $(ASM_FLAGS) -o $(LIB_DIR)/$@ $<

$(LIB_DIR)/string.o:
	$(MAKE) -C $(STRING_DIR) GCC=$(GCC) AS=$(AS)

$(LIB_DIR)/bitmap.o:
	$(MAKE) -C $(BITMAP_DIR) GCC=$(GCC) AS=$(AS)

$(LIB_DIR)/list.o:
	$(MAKE) -C $(LIST_DIR) GCC=$(GCC) AS=$(AS)

# 文件不在这里生成，无需清理
