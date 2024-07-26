ENTRY(kernel_main)    /* arm入口要设置栈指针，建立C运行环境，这个在x86里面是在MBR和Loader里面完成的 */

SECTIONS
{
    . = 0x4000e000;  /* 内核加载地址，程序各个段物理地址的起算点，也是入口函数所在的地方 */
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
    .drivers : {    /* 这个段用于加载驱动 */
        __drivers_start = .;
        KEEP(*(.drivers))
        __drivers_end = .;
    }
}
