#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"
#include "terminal.h"

void init_keyboard(void);

extern void keyboard_handler(void);

int32_t special_scancode_handler(uint8_t scan_code);

int32_t copy_buffer(void* buf);
#endif

