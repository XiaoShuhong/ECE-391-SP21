#include "x86_desc.h"

.global page_on


page_on:
    pushal
    pushfl     /*save all registers*/

    movl PD,%eax 
    movl %eax,%cr3   /*put base address of PD to the cr3 register, so that cpu know the position of PD*/

    mov %cr0,%eax
    orl 0x80000001,%eax
    movl %eax,%cr0      /*set the paging (PG) and protection (PE) bits of CR0.*/

    movl %cr4,%eax
    orl 0x00000010,%eax
    movl %eax,%cr4      /*To enable PSE (4 MiB pages) the following code is required.*/


    popfl
    popal


