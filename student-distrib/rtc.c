//rtc.c

/*Version 1 :LYC 2021/3/21 13:47*/
/*Version 2 :ML 2021/3/28 9:10*/

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

/*Version 2 ML*/
static volatile int interrupt_flag = 0;
/*Version 2 ML*/

/* init_rtc()
 * 
 * This function is used to init the RTC
 * Inputs: None
 * Outputs: None
 * Side Effects: Enable the RTC the RTC interrupt( open irq 8)
 */
void init_rtc(){
    printf("Init Real Time Clock...\n");
    //reference:https://wiki.osdev.org/RTC
    cli();
    
    /*Version 2 ML*/
    //turn on the oscillator
    outb(RTC_register_A , RTC_register_number_port);
    outb(RTC_RAM , RTC_register_W_R_port);
    /*Version 2 ML*/

    //turn on the periodic interrupt:
    outb(RTC_register_B , RTC_register_number_port);//select register B, and disable NMI
    char prev=inb(RTC_register_W_R_port);//read the current value of register B
    outb(RTC_register_B , RTC_register_number_port);// set the index again (a read will reset the index to register B)
    outb(prev | init_num , RTC_register_W_R_port);// write the previous value ORed with 0x40. This turns on bit 6 of register B
    
    
    //below is insurance, we read Register C after initialising since we are using Bochs
    outb(RTC_register_C_with_out_disable_NMI , RTC_register_number_port);
    inb(RTC_register_W_R_port);

    /*Version 2 ML*/
    //no interrupt signal
    interrupt_flag = 0;
    /*Version 2 ML*/

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
    outb(RTC_register_C_with_out_disable_NMI , RTC_register_number_port);
    inb(RTC_register_W_R_port);



   /*Version 2 ML*/
    //turn on interrupt signal
    interrupt_flag = 1;
    /*Version 2 ML*/

    send_eoi(rtc_irq_number); //send EOI to tell we have dealed with the interrupt handler
    
    sti();
}


/*Version 2 ML*/
/*
 * freq_to_rate(freq)
 * DESCRIPTION: change frequency power of 2 to rate
 * Inputs: frequency of RTC in HZ (power of 2)
 * Output: None
 * Return: the rate if success, -1 if not fail
 */
int32_t
freq_2_rate(uint32_t freq){
    /*
    freq = 2     -->  rate = 0x0F
    freq = 4     -->  rate = 0x0E
    freq = 8     -->  rate = 0x0D
    freq = 16    -->  rate = 0x0C
    freq = 32    -->  rate = 0x0B
    freq = 64    -->  rate = 0x0A
    freq = 128   -->  rate = 0x09
    freq = 256   -->  rate = 0x08
    freq = 512   -->  rate = 0x07
    freq = 1024  -->  rate = 0x06
    */
    // check whether the freq is power of 2
    uint32_t freq_minus_one = freq - 1;
    if ((freq & freq_minus_one) != 0){ return NOT_SUCCESS; }

    uint32_t temp = TWO_POWER_FIFTEEN;
    uint32_t rate = 0;
    while ((freq & temp) == 0){
        rate = rate + 1;
        temp = temp >> 1;
    }
    return rate;  
}

/*
 * set_rtc_rate(rate)
 * DESCRIPTION: change the rate of RTC
 * Inputs: rate: the rate which we want to set
 * Output: None
 * Return: None
 * SideEffect: change the rate of RTC by modify the setting of Register A
 */
void
set_rtc_rate(uint32_t rate){
    uint8_t prevA;
    
    // deal with the rate, ensure that rate must be larger equal than 2 and less equal than 15
    rate = rate & THE_F;

    cli();

    //turn on the oscillator
    outb(RTC_register_A , RTC_register_number_port);

    //write rate to register A
	prevA = inb(RTC_register_W_R_port);
    prevA = prevA & REVERSE_F;
	prevA = prevA | rate;

	outb(RTC_register_A , RTC_register_number_port);
	outb(prevA , RTC_register_W_R_port);
    sti();
}


/*
 * rtc_open()
 * DESCRIPTION: initializes RTC frequency to 2HZ
 * Inputs: the filename
 * Output: None
 * Return: 0 for success, -1 for fail as file not exist or descriptor aren't available
 * SideEffect: the rate of RTC is set to 2HZ
 */
int32_t 
rtc_open(const uint8_t* filename){

    int32_t rate = freq_2_rate(two_HZ);
    set_rtc_rate(rate);

    //no interrupt signal
    interrupt_flag = 0;
    return SUCCESS;
}

/*
 * rtc_close(fd)
 * DESCRIPTION: does nothing
 * Inputs: fd
 * Output: None
 * Return: 0 for success, -1 for fail as file not exist or descriptor aren't available
 */
int32_t
rtc_close(int32_t fd){
    return SUCCESS;
}

/*
 * rtc_read(fd, buf, nbytes)
 * DESCRIPTION: block until the next interrupt, return 0
 * Inputs: 
 *          int32_t fd - a file descriptor that was assigned in rtc_open
 *          void* buf -  not used just for system call syntax
 *          int32_t nbytes - not used just for system call syntax
 * Output: None
 * Return: 0
 */
int32_t
rtc_read(int32_t fd, void* buf, uint32_t nbytes)
{
	interrupt_flag = 0;
	while(interrupt_flag == 0);
	interrupt_flag = 0;
	return SUCCESS;
}

/*
 * rtc_write
 * DESCRIPTION: change frequency, return 0 or -1
 * Inputs: 
 *          int32_t fd - a file descriptor that was assigned in rtc_open
 *          const void* buf -  store the frequency of the RTC
 *          int32_t nbytes - number of bytes in the buffer 
 * Output: None
 * Return: number of bytes written for success, -1 for fail as file not exist or descriptor aren't available
 */
int32_t 
rtc_write(int32_t fd, const void* buf, int32_t nbytes){

    //check whether the number of bytes is 4
    if (nbytes != 4)
        return NOT_SUCCESS;
    
    //get the frequency from the buffer
    volatile int32_t freq = *((int32_t*) buf);

    //init a rate to use
    volatile uint32_t rate = 0;

    //get the rate from the frequency
    rate = freq_2_rate(freq);

    //frequency is not valid
    if (rate == NOT_SUCCESS)
        return NOT_SUCCESS;

    set_rtc_rate(rate);

    interrupt_flag = 0;
    return SUCCESS;

}

/*Version 2 ML*/
/*Version 1 LYC*/
