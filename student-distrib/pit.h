#ifndef _PIT_H
#define _PIT_H

#define PIT_COMMAND_PORT 0x43
#define PIT_DATA_PORT_CHANNEL0 0x40
#define PIT_DATA_PORT_CHANNEL1 0x41
#define PIT_DATA_PORT_CHANNEL2 0x42
#define pit_irq_number 0

#define PIT_MODE 0x37 // 0011 0111 (00 for channel 0, 11 for RWL, 0110 for mode3)
#define LATCH 11932   // ((CLOCK_TICK_RATE + Hz / 2) / Hz)
                      // CLOCK_TICK_RATE = 1193180

#endif





