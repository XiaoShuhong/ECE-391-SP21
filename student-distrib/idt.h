/*Version 1 :LYC 2021/3/21 13:47*/
#ifndef IDT__
#define IDT__

/*Version 1 LYC*/
#define RTC_IDT_INDEX 0x28
#define KEYBOARD_IDT_INDEX 0x21
//extern void rtc_interrupt_handler();// this function is defined in rtc.S 
 /*Version 1 LYC*/


extern void init_idt();



#endif


