/*Version 1 ML 2021/3/21 10:28*/

/*Version 1 ML*/
#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"

void init_keyboard(void);

extern void keyboard_handler(void);

int32_t special_scancode_handler(uint8_t scan_code);

int32_t copy_buffer(void* buf);
#endif
/*Version 1 ML*/

