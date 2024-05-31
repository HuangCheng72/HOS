[bits 32]   ; 显式指定为32位汇编

section .text

global switch_esp_virtual_addr

switch_esp_virtual_addr:
    ; Step 1: 计算当前栈顶（esp）到栈底（0x900处）的距离
    mov eax, esp            ; 保存当前栈指针到 EAX
    mov ebx, 0x0900         ; 栈底地址
    sub ebx, eax            ; 计算数据块大小（ebx = 0x0900 - 当前 ESP）

    ; Step 2: 搬移数据
    ; 数据源是esp到0x900（不包括0x900处），目的地是esp + 0xc00 到 0x1500（不包括0x1500）
    mov esi, eax            ; 源地址为当前 ESP
    add eax, 0x0c00         ; 新栈顶地址
    mov edi, eax            ; 目标地址为新 ESP
    mov ecx, ebx            ; 设置需要复制的字节数
    cld                     ; 清除方向标志位，确保数据从低地址到高地址搬移
    rep movsb               ; 复制数据块到新位置

    ; Step 3: 更新 ESP 到新的虚拟地址
    add esp, 0xc0000c00     ; 切换 ESP 到高地址（esp = 当前 esp + 0xC0000C00）
    ret                     ; 返回
