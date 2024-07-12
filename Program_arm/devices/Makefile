
# 所有设备目录
DEVICE_DIR = .
TIMER_DIR = timer

# 标签标记所有的任务（输出什么文件），方便修改
DEVICE_OBJS = timer.o

.PHONY: all

all: $(DEVICE_OBJS)

# 执行各子模块编译
timer.o:
	$(MAKE) -C $(TIMER_DIR) GCC=$(GCC) AS=$(AS) MAKE=$(MAKE)

# 清理规则
clean:
	@if exist $(DEVICE_DIR)\\*.o del $(DEVICE_DIR)\\*.o
	@if exist $(DEVICE_DIR)\\*.bin del $(DEVICE_DIR)\\*.bin
