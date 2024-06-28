# 目录
LIB_DIR = ..
LIB_KERNEL = .
PRINT_DIR = print
STRING_DIR = string
BITMAP_DIR = bitmap
LIST_DIR = list

# 汇编标志
ASM_FLAGS = -o

# 定义源文件和目标文件
SRCS = $(wildcard *.asm)
OBJS = $(SRCS:.asm=.o)

# 默认目标，编译所有的 .asm 文件
all: $(OBJS) print.o string.o bitmap.o list.o

# 目标文件生成规则
%.o: %.asm
	$(AS) $(ASM_FLAGS) $(LIB_DIR)/$@ $<

print.o:
	$(MAKE) -C $(PRINT_DIR) GCC=$(GCC) AS=$(AS) MAKE=($MAKE)

string.o:
	$(MAKE) -C $(STRING_DIR) GCC=$(GCC) AS=$(AS)

bitmap.o:
	$(MAKE) -C $(BITMAP_DIR) GCC=$(GCC) AS=$(AS)

list.o:
	$(MAKE) -C $(LIST_DIR) GCC=$(GCC) AS=$(AS)

# 文件不在这里生成，无需清理
