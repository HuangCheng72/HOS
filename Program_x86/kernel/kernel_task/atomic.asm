[bits 32]
section .text

; 这个汇编写的是自旋锁和原子操作需要用的原子性的汇编函数

global atomic_test_and_set
global automic_cas

; 实现自旋锁需要的经典test_and+set函数
; uint32_t atomic_test_and_set(volatile uint32_t *ptr, uint32_t value);
atomic_test_and_set:
    ; 函数参数：
    ; ptr: [esp + 4]
    ; value: [esp + 8]

    push ebx                ; 下面的计算过程需要用到ebx，这个寄存器是调用者负责保存的，因此这里需要保存

    mov ebx, [esp + 8]      ; 将 ptr（第一个参数）加载到 ebx 中
    mov eax, [esp + 12]     ; 将 value（第二个参数）加载到 eax 中，准备交换

    xchg eax, [ebx]         ; 原子交换操作，将 eax 中的 value 与 ebx 指向的地址中的值交换
                            ; 这一步完成后，eax 中将保存 ptr 指向地址的原始值

    pop ebx                 ; 恢复先前保存的ebx的值
                            ; 返回值在 eax 中（原来的 ptr 指向地址的值）

    ret

; 实现原子操作需要的经典CAS（compare-and-swap）函数
; uint32_t automic_cas(volatile uint32_t *ptr, uint32_t old_val, uint32_t new_val);
automic_cas:
    ; 输入：
    ;   ptr (内存地址) 存放在 [esp + 4]
    ;   old_val (预期的旧值) 存放在 [esp + 8]
    ;   new_val (要写入的值) 存放在 [esp + 12]

    push ecx
    push ebx                ; 保存寄存器 ebx和ecx，因为我们将在后面使用它

    mov ebx, [esp + 8]      ; 将 ptr 加载到 ebx 寄存器中
    mov eax, [esp + 12]     ; 将 old_val 加载到 eax 寄存器中（返回值需要）
    mov ecx, [esp + 16]     ; 将 new_val 加载到 ecx 寄存器中

    ; 现在有：
    ; ebx = ptr
    ; eax = old_val
    ; ecx = new_val

    ; cmpxchg 作用是实现原子比较并交换操作（它的比较操作参数固定为eax寄存器）
    cmpxchg [ebx], ecx      ; 如果 [ebx] == eax, 则 [ebx] = ecx；否则，eax = [ebx]

    ; 返回值会在 EAX 中，EAX 中保存的是 ptr 原来的值
    pop ebx                  ; 恢复 ebx 寄存器
    pop ecx                  ; 恢复 ecx 寄存器

    ret                      ; 返回eax中的值
