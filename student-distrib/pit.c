/*pit.c used to init periodic interval timer device*/
/*Version 1 ML 2021/4/25 9:50*/

#include "pit.h"
#include "types.h"
#include "i8259.h"
#include "lib.h"


/* void init_pit(void)
 * Description: init the periodic interval timer device and set the frequency to 100Hz
 * Inputs: None
 * Outputs: None
 * Return value: None
 * Side effects: Allow the pit interrupts with the frequency at 100Hz
 */

void
init_pit(void){
    outb(PIT_MODE, PIT_COMMAND_PORT);
    outb((uint8_t)LATCH && 0xff, PIT_DATA_PORT_CHANNEL0)
    outb((uint8_t)LATCH >> 8, PIT_DATA_PORT_CHANNEL0);

    enable_irq(pit_irq_number);
}


/* void pit_handler(void)
 * Description: handle the pit interrupt, and 
 * Inputs: None
 * Outputs: None
 * Return value: None
 * Side effects: 
 */
void
pit_handler(void){
    send_eoi(pit_irq_number);

}


