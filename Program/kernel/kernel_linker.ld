ENTRY(kernel_main)    /* 入口函数，这里将决定编出的程序入口是哪个地方 */

SECTIONS
{
    . = 0x00001500;  /* 内核加载地址，程序各个段物理地址的起算点，也是入口函数所在的地方 */
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}
