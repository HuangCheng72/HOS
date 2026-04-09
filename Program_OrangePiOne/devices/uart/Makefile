
# 目录
DEVICE_DIR = ..
UART_DIR = .

# 源文件
UART_C_SRC = $(UART_DIR)/uart.c

# C 编译标志
CFLAGS = -ffreestanding -nostdlib -Wall -Wextra

# 标签标记所有的任务（输出什么文件），方便修改
DEVICE_OBJS = $(DEVICE_DIR)/uart.o

.PHONY: all

all: $(DEVICE_OBJS)

# 编出来放到模块文件夹devices去，别放在这里，方便主Makefile找
$(DEVICE_DIR)/uart.o: $(UART_C_SRC)
	$(GCC) $(CFLAGS) -c -o $@ $<

# 文件不生成于此，无需清理规则
