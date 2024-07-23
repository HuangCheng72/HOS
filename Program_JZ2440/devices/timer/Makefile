
# 目录
DEVICE_DIR = ..
TIMER_DIR = .

# 源文件
TIMER_C_SRC = $(TIMER_DIR)/timer.c

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra

# 标签标记所有的任务（输出什么文件），方便修改
DEVICE_OBJS = $(DEVICE_DIR)/timer.o

.PHONY: all

all: $(DEVICE_OBJS)

# 编出来放到模块文件夹devices去，别放在这里，方便主Makefile找
$(DEVICE_DIR)/timer.o: $(TIMER_C_SRC)
	$(GCC) $(CFLAGS) -c -o $@ $<

# 文件不生成于此，无需清理规则
