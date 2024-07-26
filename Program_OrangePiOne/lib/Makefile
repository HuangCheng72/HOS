# 目录
LIB_DIR = .
LIB_KERNEL = lib_kernel

OBJS = kernel.o

# 默认目标
all: $(OBJS)

# 目标文件生成规则
kernel.o:
	$(MAKE) -C $(LIB_KERNEL) GCC=$(GCC) AS=$(AS) MAKE=$(MAKE)

# 清理目标，删除所有 .o 文件
clean:
	@if exist $(LIB_DIR)\\*.o del $(LIB_DIR)\\*.o
