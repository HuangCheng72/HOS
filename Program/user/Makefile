# 目录
USER_DIR = .
USER_PROCESS_DIR = process

OBJS = process.o

# 默认目标
all: $(OBJS)

# 目标文件生成规则
process.o:
	$(MAKE) -C $(USER_PROCESS_DIR) GCC=$(GCC) AS=$(AS) NASM=$(NASM) MAKE=$(MAKE)

# 清理目标，删除所有 .o 文件
clean:
	@if exist $(USER_DIR)\\*.o del $(USER_DIR)\\*.o
