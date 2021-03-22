/*Version 1 LYC 2021/3/21 13:47*/
/*Version 2 ZLH 2021/3/22 19:01*/
#ifndef IDT__
#define IDT__

/*Version 1 LYC*/
#define RTC_IDT_INDEX 0x28
#define KEYBOARD_IDT_INDEX 0x21
/*Version 2 ZLH*/
#define SYSTEM_IDT_INDEX 0x80
/*Version 2 ZLH*/

//extern void rtc_interrupt_handler();// this function is defined in rtc.S 
 /*Version 1 LYC*/


extern void init_idt();



#endif


