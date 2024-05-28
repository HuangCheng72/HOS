; 这个汇编是提供向硬件端口读写数据功能的

[bits 32]   ; 显式指定为32位汇编

section .text

global outb
global outsw
global inb
global insw

; 向端口port写入一个字节的数据
; void outb(uint16_t port, uint8_t data)
outb:
    mov edx, [esp + 4]      ; 将 port 移动到 edx
    mov al, [esp + 8]       ; 将 data 移动到 eax
    out dx, al              ; 将一个字节输出到端口
    ret

; 将addr处起始的word_cnt个字写入端口port
; void outsw(uint16_t port, const void* addr, uint32_t word_cnt)
outsw:
    mov edx, [esp + 4]         ; 将 port 移动到 edx
    mov esi, [esp + 8]         ; 将 addr 移动到 esi
    mov ecx, [esp + 12]        ; 将 word_cnt 移动到 ecx
    cld                        ; 清除方向标志
    rep outsw                  ; 重复输出字串
    ret

; 将从端口port读入的一个字节返回
; uint8_t inb(uint16_t port)
inb:
    mov dx, word [esp + 4]     ; 将 port 移动到 dx
    in al, dx                  ; 从端口输入一个字节
    movzx eax, al              ; 将 al 零扩展到 eax
    ret

; 将从端口port读入的word_cnt个字写入addr
; void insw(uint16_t port, void* addr, uint32_t word_cnt)
insw:
    mov edx, [esp + 4]         ; 将 port 移动到 edx
    mov edi, [esp + 8]         ; 将 addr 移动到 edi
    mov ecx, [esp + 12]        ; 将 word_cnt 移动到 ecx
    cld                        ; 清除方向标志
    rep insw                   ; 重复输入字串
    ret
