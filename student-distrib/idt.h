#ifndef IDT__
#define IDT__


#define RTC_IDT_INDEX 0x28
#define KEYBOARD_IDT_INDEX 0x21
#define SYSTEM_IDT_INDEX 0x80
#define PIT_IDT_INDEX 0x20
#define EXCEPTION_ERR 0xF


extern void init_idt();



#endif


