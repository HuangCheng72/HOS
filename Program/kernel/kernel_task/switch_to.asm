[bits 32]
section .text

global switch_to

switch_to:
   push esi
   push edi
   push ebx
   push ebp

   mov eax, [esp + 20]          ; 得到参数cur（4 * 4 + 返回地址4 = 20）
   mov [eax], esp               ; 解引用cur，得到第一个元素的值，也就是task的self_stack字段，栈顶指针
                                ; self_stack是struct task的第一个元素，所以不需要加偏移量
                                ; 直接对其写入esp的值即可，更新栈顶指针

;------------------  以上是备份当前任务的环境，下面是恢复下一个任务的环境  ----------------

   mov eax, [esp + 24]		    ; 得到栈中的参数next, next = [esp + 24]
   mov esp, [eax]		        ; pcb的第一个成员是self_kstack成员，记录栈顶指针
				                ; 从self_kstack中恢复到esp，恢复到这个任务的栈顶指针

				                ; 第一次上CPU执行的时候，栈顶在这里
				                ; pop四个都是0（最开始执行当然要清空）
   pop ebp
   pop ebx
   pop edi
   pop esi
   ret				            ; 此时，根据任务信息栈的设计，此时栈顶已经来到了上次执行到的入口地址
                                ; 就直接进入继续执行了
                                ; 如果是第一次上CPU的话，这里是我们初始化时候的包装函数入口地址
                                ; 所以ret就进入入口地址开始执行了
