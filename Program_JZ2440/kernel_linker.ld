ENTRY(_start)

SECTIONS {
    . = 0x00000000;
    .startup : AT(0x00000000) {
        entry/head.o
        entry/init.o
        entry/nand.o
        /* 把启动段的代码放在这里，这样就能保证这些东西在最前面了 */
    }

    /* 内核段：执行地址从 0x3000e000 开始，加载地址从 0x00001000 开始 */
    . = 0x3000e000;

    .text : AT(0x00001000) {
        *(.text)        /* 匹配所有 .text 和 .text. 开头的段 */
    }

    .rodata ALIGN(4) : AT((LOADADDR(.text) + SIZEOF(.text) + 3) & ~3) {
        *(.rodata.*)    /* 匹配所有 .rodata 和 .rodata. 开头的段 */
    }

    .data ALIGN(4) : AT((LOADADDR(.rodata) + SIZEOF(.rodata) + 3) & ~3) {
        *(.data.*)      /* 匹配所有 .data 和 .data. 开头的段 */
    }

    .bss  ALIGN(4) : AT((LOADADDR(.data) + SIZEOF(.data) + 3) & ~3) {
        *(.bss*)       /* 匹配所有 .bss 和 .bss. 开头的段 */
    }

    .drivers ALIGN(4) : AT((LOADADDR(.bss) + SIZEOF(.bss) + 3) & ~3) {
        __drivers_start = .;
        KEEP(*(.drivers))
        __drivers_end = .;
    }
}
