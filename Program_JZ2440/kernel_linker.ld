ENTRY(_start)

SECTIONS
{
    . = 0x30000000;
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
    .drivers : {
        __drivers_start = .;
        KEEP(*(.drivers))
        __drivers_end = .;
    }
}
