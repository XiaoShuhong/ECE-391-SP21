/*Version 1 ZLH 2021/3/29 18:41*/
/*Version 1 ZLH*/
#ifndef TERMINAL_H
#define TERMINAL_H

#include "types.h"




#define LINE_BUFFER_SIZE 128
#define FAIL -1
#define SUCCESS 0
// #define EMPTY '\0'
#define max_terminal_number 3

int32_t switch_terminal(int32_t terminal_number);

typedef struct TCB{
    
    char line_buffer[LINE_BUFFER_SIZE];
    volatile uint8_t stdin_enable;
    uint8_t buffer_index;
    int cursor_x;
    int cursor_y;
    uint32_t video_buff;

    /* parameters used for multi-processes control */
    // int32_t current_pid;
    struct PCB* current_process;
    
    struct TCB* next_terminal;
    int32_t tid;
    int32_t vidmap;

///////////////////////////////////////////////////
    int32_t running_pid;


} TCB;

extern int32_t terminal_read(int32_t fd, void* buf, uint32_t nbytes);
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t terminal_open(const uint8_t* filename);
extern int32_t terminal_close(int32_t fd);
extern void clear_buffer();
extern void add_buffer(uint8_t key);

char line_buffer[LINE_BUFFER_SIZE];
char terminal_buffer[LINE_BUFFER_SIZE];  
int32_t buffer_index;
int32_t terminal_read_flag;


extern TCB terminals[max_terminal_number];
extern int32_t current_terminal_number;


#endif
/*Version 1 ZLH*/



