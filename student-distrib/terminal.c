/*terminal.c used to define four terminal functions*/
/*Version 1 ZLH 2021/3/29 18:41*/
/*Version 2 ML 2021/3/22 18:51 add function header and comments*/

/*Version 1 ZLH*/
#include "terminal.h"
#include "keyboard.h"
#include "types.h"
#include "lib.h"

int32_t buffer_index = 0; // the cursor index of the buffer

/* int32_t terminal_read(fd, buf, nbytes)
 * 
 * This function is used copy the content of line buffer into the terminal buffer
 * Inputs: 
 *          fd: file discriptor
 *          buf: the terminal buffer
 *          nbytes: the number of bytes which we should copy into the buffer
 * Outputs: None
 * Return: buffer_index : the index of the cursor in the buffer
 * Side Effects: copy the content of line buffer into the terminal buffer
 */
int32_t terminal_read(int32_t fd, void* buf, uint32_t nbytes)
{
// 	if(nbytes!=LINE_BUFFER_SIZE || buf==NULL){
//         return FAIL;
//     }
// 	strncpy((int8_t*) buf, line_buffer, buffer_index);
// 	return buffer_index;
//
    terminal_read_flag = 1;
    if(buf == NULL){
        return FAIL;
    }
    return copy_buffer(buf);
}


/* int32_t term_write(fd, buf, nbytes)
 * 
 * This function is used put the content of the buffer onto the screen
 * Inputs: 
 *          fd: file discriptor
 *          buf: the terminal buffer
 *          nbytes: the number of bytes which we should put
 * Outputs: None
 * Return: temp_count : the number of bytes which we have written
 * Side Effects: put the content of the buffer onto the screen (video memory)
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
    int32_t temp_count;
    if(buf == NULL){
        return FAIL;
    }
    const char* char_buf = buf;  
    for(temp_count = 0; temp_count < nbytes; temp_count++){
        putc(char_buf[temp_count]);
    }
    /* return # of bytes written */
    return temp_count;
}

/* int32_t term_open(filename)
 * 
 * This function is used put the content of the buffer onto the screen
 * Inputs: filename: the name of the file
 * Outputs: None
 * Return: SUCCESS 0
 * Side Effects: open a file's terminal
 */
int32_t terminal_open(const uint8_t* filename) {
    return SUCCESS;
}

/* int32_t term_close(fd, buf, nbytes)
 * 
 * This function is used put the content of the buffer onto the screen
 * Inputs: 
 *          fd: file discriptor
 * Outputs: None
 * Return: SUCCESS
 * Side Effects: close the the terminal
 */
int32_t terminal_close(int32_t fd) {
    return SUCCESS;
}

/* void clear_buffer()
 * 
 * This function is used to clear the content of the buffer
 * Inputs: None
 * Outputs: None
 * Return: None
 * Side Effects: clear the buffer
 */
void clear_buffer(){
    int32_t i;
    if (line_buffer == NULL) {
        return;
    }
    for(i = 0; i<LINE_BUFFER_SIZE;i++){
        line_buffer[i] = '\0';
    }
    buffer_index = 0;
}

/* void add_buffer()
 * 
 * This function is used to add a key in th buffer
 * Inputs: uint8_t key: the key which we want to add to the buffer
 * Outputs: None
 * Return: None
 * Side Effects: add a key into the buffer
 */
void add_buffer(uint8_t key){
    if (line_buffer == NULL) {
        return;
    }
    if(buffer_index == LINE_BUFFER_SIZE){
        return;
    }
    if((buffer_index == LINE_BUFFER_SIZE - 1) && (key != '\n')){
        return;
    }

    line_buffer[buffer_index] = key;
    buffer_index = buffer_index + 1;
}
/*Version 1 ZLH*/



TCB terminals[max_terminal_number];
int32_t current_terminal_number = 0;



int32_t
init_terminal_structure(){
    int32_t i; // index loop;
    for(i=0; i<max_terminal_number; i++){
        terminals[i].stdin_enable = 0;
        terminals[i].buffer_index = 0;
        terminals[i].cursor_x = 0;
        terminals[i].cursor_y = 0;
        terminals[i].current_pid = -1;
        terminals[i].current_process = NULL;
        terminals[i].tid = i;
        terminals[i].next_terminal = &terminals[(i+1)%max_terminal_number];
        terminals[i].vidmap = 0;
    }
}

int32_t
switch_terminal(int32_t terminal_number){

    /* if the terminal is already the terminal_number th terminal, return -1 */
    if (terminal_number == current_terminal_number){
        return FAIL;
    }

    /* from the terminals table, get the current and new terminal */
    TCB* current_terminal_pointer = &terminals[current_terminal_number];
    TCB* new_terminal_pointer = &terminals[terminal_number];

    /* save the buffer index*/
    current_terminal_pointer->buffer_index = buffer_index;

    /* load the new terminal's information */
    buffer_index = new_terminal_pointer->buffer_index;
    *(line_buffer) = new_terminal_pointer->line_buffer;

    screen_x = new_terminal_pointer->cursor_x;
    screen_y = new_terminal_pointer->cursor_y;
    update_cursor(screen_x, screen_y);
    
    /* switch the video memory */
    current_terminal_number = terminal_number;


}






