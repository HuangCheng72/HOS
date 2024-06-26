# 主Makefile，用于整合各模块操作

# 全套arm编译工具链
GCC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

# 项目构建工具make
MAKE = make

# 因为这个操作系统本质上是个宏内核，所有的组件都要打包在一个kernel.bin里面
# 这样烧写起来也方便，所以要在主Makefile这里统一链接

# 所有组件目录
BOOT_DIR = boot
KERNEL_DIR = kernel
LIB_DIR = lib
DEVICE_DIR = devices
USER_DIR = user
FS_DIR = fs

# ARM需要的中间文件
KERNEL_ELF = $(KERNEL_DIR)/kernel.elf

# 输出文件
KERNEL_BIN = kernel.bin

# 链接脚本位置
KERNEL_LINKER_SCRIPT = $(KERNEL_DIR)/kernel_linker.ld

# 链接标志
LDFLAGS = -T $(KERNEL_LINKER_SCRIPT)

.PHONY: all

# 编译各组件，要把工具参数传输过去，然后主Makefile汇总链接生成 kernel.bin
all: compile link bin

# 编译各组件
compile:
	$(MAKE) -C $(BOOT_DIR) GCC=$(GCC) AS=$(AS) MAKE=$(MAKE)
#	$(MAKE) -C $(LIB_DIR) GCC=$(GCC) AS=$(AS) MAKE=$(MAKE)
	$(MAKE) -C $(KERNEL_DIR) GCC=$(GCC) AS=$(AS) MAKE=$(MAKE)
#	$(MAKE) -C $(DEVICE_DIR) GCC=$(GCC) AS=$(AS) MAKE=$(MAKE)
#	$(MAKE) -C $(USER_DIR) GCC=$(GCC) AS=$(AS) MAKE=$(MAKE)
#	$(MAKE) -C $(FS_DIR) GCC=$(GCC) AS=$(AS) MAKE=$(MAKE)

# 链接生成 kernel.elf
link:
	$(LD) $(LDFLAGS) -o $(KERNEL_ELF) $(wildcard $(BOOT_DIR)/*.o) \
										$(wildcard $(KERNEL_DIR)/*.o)
#										$(wildcard $(LIB_DIR)/*.o) \
#										$(wildcard $(DEVICE_DIR)/*.o) \
#										$(wildcard $(USER_DIR)/*.o) \
#										$(wildcard $(FS_DIR)/*.o)

# 提取二进制文件kernel.bin
bin:
	$(OBJCOPY) -O binary $(KERNEL_ELF) $(KERNEL_BIN)

clean:
	$(MAKE) -C $(BOOT_DIR) clean
	$(MAKE) -C $(KERNEL_DIR) clean
	$(MAKE) -C $(LIB_DIR) clean
	$(MAKE) -C $(DEVICE_DIR) clean
	$(MAKE) -C $(USER_DIR) clean
	$(MAKE) -C $(FS_DIR) clean
