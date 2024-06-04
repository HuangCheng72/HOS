
# 所有设备目录
DEVICE_DIR = .
PIT8253_DIR = pit8253
CONSOLE_DIR = console
KEYBOARD_DIR = keyboard

# 标签标记所有的任务（输出什么文件），方便修改
DEVICE_OBJS = pit8253.o console.o keyboard.o

.PHONY: all

all: $(DEVICE_OBJS)

# 执行各子模块编译
pit8253.o:
	$(MAKE) -C $(PIT8253_DIR) GCC=$(GCC) AS=$(AS)

console.o:
	$(MAKE) -C $(CONSOLE_DIR) GCC=$(GCC) AS=$(AS)

keyboard.o:
	$(MAKE) -C $(KEYBOARD_DIR) GCC=$(GCC) AS=$(AS)

# 清理规则
clean:
	@if exist $(DEVICE_DIR)\\*.o del $(DEVICE_DIR)\\*.o
	@if exist $(DEVICE_DIR)\\*.bin del $(DEVICE_DIR)\\*.bin