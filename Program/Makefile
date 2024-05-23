# 主Makefile，用于整合各模块操作

# 汇编器和编译器、链接器等各种工具
NASM = nasm
GCC = i386-elf-gcc
AS = i386-elf-as
LD = i386-elf-ld
MAKE = make

# 因为这个操作系统本质上是个宏内核，所有的组件都要打包在一个kernel.bin里面
# 这样烧写起来也方便，所以要在主Makefile这里统一链接

# 所有组件目录
BOOT_DIR = boot
KERNEL_DIR = kernel

# 输出文件
KERNEL_BIN = $(KERNEL_DIR)/kernel.bin

# 链接脚本位置
KERNEL_LINKER_SCRIPT = $(KERNEL_DIR)/kernel_linker.ld

# 链接标志
LDFLAGS = -T $(KERNEL_LINKER_SCRIPT) --oformat binary

.PHONY: all

# 编译各组件，要把工具参数传输过去，然后主Makefile汇总链接生成 kernel.bin
all:
	$(MAKE) -C $(BOOT_DIR) NASM=$(NASM)
	$(MAKE) -C $(KERNEL_DIR) NASM=$(NASM) GCC=$(GCC) AS=$(AS)
	$(LD) $(LDFLAGS) -o $(KERNEL_BIN) $(KERNEL_DIR)/kernel.o $(KERNEL_DIR)/kernel_func.o


clean:
	$(MAKE) -C $(BOOT_DIR) clean
	$(MAKE) -C $(KERNEL_DIR) clean
