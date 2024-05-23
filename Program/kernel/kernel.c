#include "type.h"

extern void print_char(char c, uint16_t pos);
extern void io_hlt();

void kernel_main(void) {
    print_char('K', 160);
    
    // 进入内核主循环或其它初始化代码
    for(;;) {
        io_hlt();
    }
}
