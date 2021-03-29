/*Version 1 ZLH 2021/3/29 18:41*/
/*Version 1 ZLH*/
#ifndef TERMINAL_H
#define TERMINAL_H

#include "types.h"




#define LINE_BUFFER_SIZE 128
#define FAIL -1
#define SUCCESS 0
// #define EMPTY '\0'

extern int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t terminal_open(const uint8_t* filename);
extern int32_t terminal_close(int32_t fd);
extern void clear_buffer();
extern void add_buffer(uint8_t key);

char line_buffer[LINE_BUFFER_SIZE];
char terminal_buffer[LINE_BUFFER_SIZE];  
int32_t buffer_index;
int32_t terminal_read_flag;


#endif
/*Version 1 ZLH*/



