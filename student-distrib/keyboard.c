/*keyboard.c used to init keyboard device*/
/*Version 1 ML 2021/3/21 10:28*/
/*Version 2 ML 2021/3/22 20:01 add function header and comments*/

/*Version 1 ML*/
/*Version 2 ML*/
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "keyboard.h"


#define KEYBOARD_IRQ 1
#define KEYBOARD_PORT_DATA 0x60

#define SCANCODESIZE 58
#define SINGLECOMMA  39
#define DOUBLECOMMA  34

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
void
keyboard_handler(void){
    cli();
    uint8_t scan_code = inb(KEYBOARD_PORT_DATA);
    if (scan_code < SCANCODESIZE && scan_code > 0x01){
        putc(key2ascii_map[scan_code][0]);
    }
    send_eoi(KEYBOARD_IRQ); // send the signal of end_of_interrupt
    sti();
}
/*Version 1 ML*/
/*Version 2 ML*/
