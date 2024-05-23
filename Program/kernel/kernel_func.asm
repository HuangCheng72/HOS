[bits 32]   ; 显式指定为32位汇编

section .text
global print_char, io_hlt

; 在屏幕上打印字符
; void print_char(char c, uint16_t pos);
print_char:
    ; 函数调用开始时栈布局：
    ; +------------------+
    ; | 参数2            | <-- ESP + 4 + 4 = ESP + 8（这里是栈底，之前ESP在这）
    ; +------------------+
    ; | 参数1            | <-- ESP + 4（以在底层都是指针传递，所以必须是4个字节）
    ; +------------------+
    ; | 返回地址         | <-- ESP（就是所谓的栈顶）
    ; +------------------+
    
    
    push eax                    ; 保存 eax
    push edi                    ; 保存 edi

    ; 压栈后栈布局：
    ; +------------------+
    ; | 参数2            | <-- ESP + 12 + 4 = ESP + 16
    ; +------------------+
    ; | 参数1            | <-- ESP + 8 + 4 = ESP + 12
    ; +------------------+
    ; | 返回地址         | <-- ESP + 4 + 4 = ESP + 8
    ; +------------------+
    ; | 保存的 eax       | <-- ESP + 4 (push eax)
    ; +------------------+
    ; | 保存的 edi       | <-- ESP (push edi)
    ; +------------------+

    mov al, [esp + 12]           ; 获取第一个参数 
    mov di, [esp + 16]          ; 获取第二个参数 

    ; 将字符写入显存
    mov [gs:di], al
    add di, 2                   ; 每个字符占两个字节

    pop edi                     ; 恢复 edi
    pop eax                     ; 恢复 eax
    ret


; 让CPU停下
; void io_hlt();
io_hlt:
    hlt
    ret
