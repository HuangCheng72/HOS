[bits 32]   ; 显式指定为32位汇编

section .text

global switch_esp_virtual_addr

switch_esp_virtual_addr:
    add esp, 0xc0000000 ; 将ESP指针放到虚拟地址上
    ret
