[bits 32]   ; 显式指定为32位汇编

section .text

global load_tss

;void load_tss(uint16_t tss_selector);
load_tss:
    ; 将TSS段选择子保存到寄存器 bx 中
    mov bx, word [esp + 4]   ; 参数从堆栈中取出，偏移量为4

    ; 使用 ltr 指令加载TSS段选择子到 TR 寄存器
    ltr bx

    ret   ; 返回

