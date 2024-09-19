.section .text

.global atomic_test_and_set
.global automic_cas

// uint32_t atomic_test_and_set(volatile uint32_t *ptr, uint32_t value)//
atomic_test_and_set:
    // 函数参数：
    // ptr: r0 (第一个参数，指向内存的指针)
    // value: r1 (第二个参数，将要交换的值)

    push {r2, r3}            // 保存r2和r3

    mov r2, r1               // 将value存储到r2中
    ldrex r1, [r0]           // 加载ptr指向的值到r1，并标记为独占访问
    strex r3, r2, [r0]       // 尝试将r2的值存储到ptr，r3会表示是否成功

    cmp r3, #0               // 检查存储是否成功，strex的返回值为0表示成功
    bne atomic_test_and_set  // 如果存储失败（非0），则继续尝试

    mov r0, r1               // 将原来的值存储到r0作为返回值

    pop {r2, r3}             // 恢复r2和r3
    bx lr                    // 返回

// uint32_t automic_cas(volatile uint32_t *ptr, uint32_t old_val, uint32_t new_val)//
automic_cas:
    // ptr: r0 (第一个参数)
    // old_val: r1 (第二个参数)
    // new_val: r2 (第三个参数)

    push {r3}                // 保存r3

    ldrex r3, [r0]           // 加载ptr指向的值到r3，并标记为独占访问
    cmp r3, r1               // 比较r3（ptr的当前值）与r1（old_val）
    bne cas_fail             // 如果不相等，则跳转到失败分支

    strex r3, r2, [r0]       // 尝试将new_val存储到ptr中，r3会表示是否成功
    cmp r3, #0               // 检查存储是否成功
    bne automic_cas          // 如果失败（非0），继续尝试

cas_fail:
    mov r0, r3               // 将ptr原始的值存入r0作为返回值

    pop {r3}                 // 恢复r3
    bx lr                    // 返回
