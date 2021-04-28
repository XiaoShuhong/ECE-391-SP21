/*keyboard.c used to init keyboard device*/
/*Version 1 ML 2021/3/21 10:28*/
/*Version 2 ML 2021/3/22 20:01 add function header and comments*/
/*Version 3 ML 2021/3/27 16:22*/
/*Version 4 ZLH 2021/3/29 20:56*/


/*Version 1 ML*/
/*Version 2 ML*/
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#include "types.h"
#include "terminal.h"
#include "pit.h"


#define KEYBOARD_IRQ 1
#define KEYBOARD_PORT_DATA 0x60

#define SCANCODESIZE 58
#define SINGLECOMMA  39
#define DOUBLECOMMA  34
#define LOW_EIGHT_BITS 0xFF

#define LEFT_SHIFT_PRESSED 0x2A
#define RIGHT_SHIFT_PRESSED 0x36
#define LEFT_SHIFT_RELEASED 0xAA
#define RIGHT_SHIFT_RELEASED 0xB6
#define LEFT_CTRL_PRESSED 0x1D
#define LEFT_CTRL_RELEASED 0x9D
#define CAPSLOCK_PRESSED 0x3A
#define CAPSLOCK_RELEASED 0xBA
#define ALT_PRESSED 0x38
#define ALT_RELEASED 0xB8
#define F1  0x3B
#define F2  0x3C
#define F3  0x3D
#define ESC 0x01
#define TAB_PRESSED 0x0F

unsigned char key2ascii_map[SCANCODESIZE][2] = {
    {0x0, 0x0}, // 0x00 not use
    {0x0, 0x0}, // 0x01 esc
    /* 0x02 - 0x0e, "1" to backspace */
    {'1', '!'}, 
    {'2', '@'},
    {'3', '#'}, 
    {'4', '$'},
    {'5', '%'},
    {'6', '^'},
    {'7', '&'}, 
    {'8', '*'},
    {'9', '('}, 
    {'0', ')'},
    {'-', '_'}, 
    {'=', '+'},
    {'\b', '\b'}, //backspace
    /* 0x0f - 0x1b, tab to "}" */
    {' ', ' '}, // tab
    {'q', 'Q'}, 
    {'w', 'W'},
    {'e', 'E'}, 
    {'r', 'R'},
    {'t', 'T'}, 
    {'y', 'Y'},
    {'u', 'U'}, 
    {'i', 'I'},
    {'o', 'O'}, 
    {'p', 'P'},
    {'[', '{'}, 
    {']', '}'},
    /* 0x1c - 0x28, enter to "'"*/
    {'\n', '\n'}, // enter
    {0x0, 0x0}, // Left Ctrl
    {'a', 'A'},
    {'s', 'S'},
    {'d', 'D'}, 
    {'f', 'F'},
    {'g', 'G'}, 
    {'h', 'H'},
    {'j', 'J'}, 
    {'k', 'K'},
    {'l', 'L'}, 
    {';', ':'},
    {SINGLECOMMA, DOUBLECOMMA},
    /* 0x29 - 0x39 "`" to Spacebar*/
    {'`', '~'},
    {0x0, 0x0}, // Left Shift
    {'\\', '|'},
    {'z', 'Z'}, 
    {'x', 'X'},
    {'c', 'C'}, 
    {'v', 'V'},
    {'b', 'B'}, 
    {'n', 'N'},
    {'m', 'M'}, 
    {',', '<'},
    {'.', '>'}, 
    {'/', '?'},
    {0x0, 0x0}, // Right Shift
    {0x0, 0x0},
    {0x0, 0x0}, 
    {' ', ' '}, 

};

/*Version 3 ML*/
/* Define the buffers to detect whether the special scancode is inputted */
/* 0 is unpressed, 1 is pressed*/
uint8_t Ctrl_Buffer = 0;
uint8_t Shift_Buffer = 0;
uint8_t CapsLock_Buffer = 0;
uint8_t Backspace_Buffer = 0;
uint8_t Alt_Buffer = 0;
/* Define the state buffer to record the state of CapsLock */
/* 0 is lowercases, 1 is capitals*/
uint8_t CapsLock_state = 0;

/*Version 3 ML*/

/* init_keyboard()
 * 
 * This function is used init the keyboard device
 * Inputs: None
 * Outputs: None
 * Side Effects: Enable the keyboard begins the interrupt
 */
void
init_keyboard(void){
    // cli();
    printf("Init keyboard...\n");
    enable_irq(KEYBOARD_IRQ);
    // sti();
}

/* keyboard_handler()
 * 
 * This function is used as the handler to handle the keyboard interrupt
 * Inputs: None
 * Outputs: None
 * Side Effects: Send the scan code which sent by the keyboard device to our video memory to show it on the screen
 */
/*Version 3 ML*/
void
keyboard_handler(void){
    int i;
    cli();
    send_eoi(KEYBOARD_IRQ); // send the signal of end_of_interrupt. This instruction must be here!!! Otherwise the eoi signal will not be sent, other interrupts will not be allowed to happen
    uint8_t scan_code = inb(KEYBOARD_PORT_DATA) & LOW_EIGHT_BITS;
    uint8_t keyprinted = key2ascii_map[scan_code][0]; // get the key which we want to printed, init to the lowercase
    
    char line_buffer[LINE_BUFFER_SIZE];
    for (i=0; i<LINE_BUFFER_SIZE; i++){
        line_buffer[i] = terminals[current_terminal_number].line_buffer[i];
    }

    int buffer_index = terminals[current_terminal_number]._buffer_index;

    /* for the special scancode, return */
    if (special_scancode_handler(scan_code) == 1) {
        sti();
        return; 
    }

    /*Version 4 ZLH*/
    if (scan_code < SCANCODESIZE && scan_code > 0x01){
     /* handle the CTRL+L, clear the screen */
        if (Ctrl_Buffer == 1){
            if (keyprinted == 'l'){
                clear();
                sti();
                return;
            }
        }

        if (keyprinted == '\b'){
            if(buffer_index == 0){
                return;
            }
            buffer_index--;
            terminals[current_terminal_number]._buffer_index--;
            line_buffer[buffer_index] = '\0';
            terminals[current_terminal_number].line_buffer[buffer_index] = '\0'; 
            backspace();
            return;
        }
    /*Version 4 ZLH*/

        if((buffer_index == 127) && (keyprinted != '\n')){
            return;
        }
    /*Version 4 ZLH*/
        if(keyprinted == '\n'){
            putc(keyprinted);
            add_buffer(line_buffer,keyprinted,buffer_index);
            buffer_index++;
            add_buffer(terminals[current_terminal_number].line_buffer,keyprinted,terminals[current_terminal_number]._buffer_index);
            terminals[current_terminal_number]._buffer_index++;
            if(terminal_read_flag == 0){ //This is used to solve the problem of terminal read when pressing enter
                clear_buffer(line_buffer);
                buffer_index = 0;
                clear_buffer(terminals[current_terminal_number].line_buffer);
                terminals[current_terminal_number]._buffer_index = 0;
            }
    /*Version 4 ZLH*/
            return;
        }

        /* handle the not special keys */
        if (keyprinted >= 'a' && keyprinted <= 'z'){ // for the letters
            if (CapsLock_state != Shift_Buffer){ // the case to print the capitals
                keyprinted = key2ascii_map[scan_code][1];
            }
        }
        else{
            if (Shift_Buffer == 1){
               keyprinted = key2ascii_map[scan_code][1]; 
            }
        }
        add_buffer(line_buffer,keyprinted,buffer_index);
        buffer_index++;
        add_buffer(terminals[current_terminal_number].line_buffer,keyprinted,terminals[current_terminal_number]._buffer_index);
        terminals[current_terminal_number]._buffer_index++;

        putc(keyprinted);
     // use to test
    }
    /*After these operations, we will get the correct printed key, what should we do next?*/
    // your code here...
    sti();
}



/* special_scancode_handler()
 * 
 * This function is used as the handler to handle the special scancode
 * Inputs: scan_code: got from the keyboard
 * Outputs: 1 for success, 0 for fail
 * Side Effects: Change the special key buffers
 */
int32_t special_scancode_handler(uint8_t scan_code){
    switch(scan_code){
        /* Shift pressed, change the ShiftBuffer to 1 */
        case LEFT_SHIFT_PRESSED:
            Shift_Buffer = 1;
            return 1;
        case RIGHT_SHIFT_PRESSED:
            Shift_Buffer = 1;
            return 1;
        /* Shift released, change the ShiftBuffer to 1 */
        case LEFT_SHIFT_RELEASED:
            Shift_Buffer = 0;
            return 1;
        case RIGHT_SHIFT_RELEASED:
            Shift_Buffer = 0;
            return 1;
        /* Ctrl pressed, change the ShiftBuffer to 1 */
        case LEFT_CTRL_PRESSED:
            Ctrl_Buffer = 1;
            return 1;
        /* Ctrl released, change the ShiftBuffer to 1 */
        case LEFT_CTRL_RELEASED:
            Ctrl_Buffer = 0;
            return 1;
        /* CapsLock pressed, change the CapsLock buffer to 1 */
        case CAPSLOCK_PRESSED:
            CapsLock_Buffer = 1;
            return 1;
        case CAPSLOCK_RELEASED:
            CapsLock_Buffer = 0;
            CapsLock_state = 1 - CapsLock_state; // change the state, if 0, change to 1, if 1, change to 0.
            return 1;
        case ALT_PRESSED:
            Alt_Buffer = 1;
            return 1;
        case ALT_RELEASED:
            Alt_Buffer = 0;
            return 1;
        case F1:
            if(Alt_Buffer==1){switch_terminal(0);}
            return 1;
        case F2:
            if(Alt_Buffer==1){switch_terminal(1);}
            return 1;
        case F3:
            if(Alt_Buffer==1){switch_terminal(2);}
            return 1;
        case ESC:
            return 1;
        case TAB_PRESSED:
            return 1;
        default:
            return 0;
    }
}
/*Version 3 ML*/

/*Version 1 ML*/
/*Version 2 ML*/

int32_t copy_buffer(void* buf){
    int32_t num;
    int i;
    while(terminals[scheduled_index].line_buffer[terminals[scheduled_index]._buffer_index - 1] != '\n'){}
    int buffer_index = terminals[scheduled_index]._buffer_index;
    num = buffer_index;

    char line_buffer[LINE_BUFFER_SIZE];
    for (i=0; i<LINE_BUFFER_SIZE; i++){
        line_buffer[i] = terminals[current_terminal_number].line_buffer[i];
    }
    strncpy((int8_t*) buf, line_buffer, buffer_index);
    clear_buffer(terminals[scheduled_index].line_buffer);
    terminals[scheduled_index]._buffer_index= 0;

    terminal_read_flag = 0;
    return num;
}


