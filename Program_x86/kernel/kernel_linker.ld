ENTRY(kernel_main)    /* 入口函数，这里将决定编出的程序入口是哪个地方 */

SECTIONS
{
    . = 0x0000e000;  /* 内核加载地址，程序各个段物理地址的起算点，也是入口函数所在的地方 */
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
    .drivers : {    /* 这个段用于加载驱动 */
        __drivers_start = .;
        KEEP(*(.drivers))
        __drivers_end = .;
    }
}
