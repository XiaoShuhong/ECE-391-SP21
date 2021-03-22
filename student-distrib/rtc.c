//rtc.c

/*Version 1 :LYC 2021/3/21 13:47*/

/*Version 1 LYC*/
#include "rtc.h"
#include "lib.h"
#include "i8259.h"


//copy from lib.c
#define VIDEO       0xB8000
#define NUM_COLS    80
#define NUM_ROWS    25
#define COLOR_BG_ATTRIB      0xF7
#define ATTRIB      0x7
#define TERMINAL_BAR 5


/* init_rtc()
 * 
 * This function is used to init the RTC
 * Inputs: None
 * Outputs: None
 * Side Effects: Enable the RTC the RTC interrupt( open irq 8)
 */
void init_rtc(){
    //reference:https://wiki.osdev.org/RTC
    cli();
    
    
    //turn on the periodic interrupt:
    outb(RTC_register_B , RTC_register_number_port);//select register B, and disable NMI
    char prev=inb(RTC_register_W_R_port);//read the current value of register B
    outb(RTC_register_B , RTC_register_number_port);// set the index again (a read will reset the index to register B)
    outb(prev | init_num , RTC_register_W_R_port);// write the previous value ORed with 0x40. This turns on bit 6 of register B
    
    
    //below is insurance, we read Register C after initialising since we are using Bochs
    outb( 0x0C,RTC_register_number_port);
    inb(RTC_register_W_R_port);


    //enable the irq8 on pic
    enable_irq(rtc_irq_number);
    sti();
}




/* __rtc_interrupt_handler__()
 * 
 * This function is used as the handler to handle the RTC interrupt
 * Inputs: None
 * Outputs: None
 * Side Effects: the characters on screen will alternate between a set of different characters at every fixed interval.
 */
void __rtc_interrupt_handler__(){
    cli();
    
    // test_interrupts();





    //if register C is not read after an IRQ 8, then the interrupt will not happen again, so we read it
    outb( 0x0C,RTC_register_number_port);
    inb(RTC_register_W_R_port);


    send_eoi(rtc_irq_number); //send EOI to tell we have dealed with the interrupt handler
    
    sti();
}

/*Version 1 LYC*/
