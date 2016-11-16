/*
 * ws2812.c
 *
 *  Created on: 2016. 8. 30.
 *      Author: HJ Park
 */




#include "stm32f4xx.h"

#define NUM_LEDs 1
#define BYTES_PER_LED 3
#define NUM_BYTES (NUM_LEDs * BYTES_PER_LED)
#define TICKS_PER_1MICROSEC 180

#if 0 //WS2812

#define T1H 126 //0.7
#define T1L 108 //0.6

#define T0H 63  //0.35  usec
#define T0L 144 //0.8

#else //WS2812B

#define T1H 144 //0.8
#define T1L 81 //0.45

#define T0H 72  //0.4  usec
#define T0L 153 //0.85


#endif

#define RESET (TICKS_PER_1MICROSEC*50)



volatile uint8_t ledBuff[NUM_BYTES];

