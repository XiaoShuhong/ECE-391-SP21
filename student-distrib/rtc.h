/*Version 1 :LYC 2021/3/21 13:47*/

/*Version 1 LYC*/
#ifndef RTC__
#define RTC__



#define RTC_register_number_port 0x70
#define RTC_register_W_R_port    0x71
#define RTC_register_A           0x8A // with disable_NMI
#define RTC_register_B           0x8B // with disable_NMI
#define RTC_register_C           0x8C // with disable_NMI
#define init_num                 0x40
#define rtc_irq_number           8
#define RTC_register_C_with_out_disable_NMI 0x0C

void init_rtc();
void __rtc_interrupt_handler__();


#endif
/*Version 1 LYC*/


