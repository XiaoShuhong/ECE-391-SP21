/*Version 1 LYC 2021/3/21 20：05*/
.global rtc_interrupt_handler,keyboard_interrupt_handler

/*Version 1 LYC*/
rtc_interrupt_handler:
    pushal
    pushfl      /*save all gereral purpose register and EGLAG*/
    call __rtc_interrupt_handler__
    popfl
    popal
    iret       /*we should return to program after interrupt*/



keyboard_interrupt_handler:
    pushal
    pushfl      /*save all gereral purpose register and EGLAG*/
    call keyboard_handler
    popfl
    popal
    iret       /*we should return to program after interrupt*/
/*Version 1 LYC*/

