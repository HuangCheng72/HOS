ENTRY(_start)

SECTIONS
{
    . = 0x30000000;
    .startup : {
        *(.text.startup)   /* 把启动段的代码放在这里，这样就能保证这些东西在最前面了 */
        *(.data.startup)
        *(.bss.startup)
    }
    . = 0x3000e000;        /* 内核段起始地址 */
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
    .drivers : {
        __drivers_start = .;
        KEEP(*(.drivers))
        __drivers_end = .;
    }
}
