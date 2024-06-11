[bits 32]   ; 显式指定为32位汇编

section .text

extern put_char
extern put_str
extern put_int

global switch_to_user_mode

; void switch_to_user_mode();

; C语言遵循cdecl调用约定标准
; 调用时候会按顺序压栈：参数（从右到左）、返回地址
switch_to_user_mode:
    ; 关闭中断，防止被切换
    cli
    ; 进入这里的时候，任务信息栈已经结束它的作用了
    ; 理论上此时esp位于中断栈的栈顶
    ; 实测结果，距离中断栈栈顶44个字节（我也不知道怎么回事，反正我是没搞明白）
    ; 实测进入switch_to_user_mode的时候，esp距离中断栈栈顶是44个字节

    ; 这里要使用中断栈里面的数据，通过iretd来跳入用户态
    ; 操作过程一团乱麻，不看了，反正实测就是76个字节就得了

    add esp, 76

    ; 跳过最开始的中断号和手动压栈错误码
    add esp, 8

    pop gs                  ; 恢复各个段寄存器
    pop fs
    pop es
    pop ds

    popad                   ; 恢复所有通用寄存器

    ; 从内核态跳入用户态，绝不可能有CPU压入的错误码，所以跳过错误码（切换到用户态错误码也没用）
    add esp, 4

    ; 先前已经做好了数据
    ; 不用使用sti开启中断，我们做好的数据里面eflags已经把中断的if位设置为1了，也就是打开中断
    ; 使用 iretd 指令切换到用户态（恢复到我们设置好的eip、cs、eflags、esp、ss）
    iretd
