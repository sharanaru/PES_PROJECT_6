/*
 * Led.c
 *
 *  Created on: 10-Dec-2019
 *      Author: Hp
 */

#include "Led.h"
#include "board.h"

void delay()
{
volatile uint64_t i=0;
while(i!=19000) //holds execution till delay is achieved
{ __asm("NOP");
i++;
  }
}
//only for kl25z

void LED_Initialise()
{
	LED_BLUE_INIT(1);
	LED_RED_INIT(1);
	LED_GREEN_INIT(1);
}
void ALLLED_OFF()
{
	LED_BLUE_OFF();
	LED_RED_OFF();
	LED_GREEN_OFF();
}
void LED_PASS()
{
ALLLED_OFF();
LED_GREEN_ON();
#ifdef ddebug
Send_String_Poll("led green on\n");
#endif
//delay();
}
void LED_PROCESS()
{
ALLLED_OFF();
LED_BLUE_ON();
#ifdef ddebug
Send_String_Poll("led blue on\n");
#endif
//delay();
}
void LED_FAIL()
{
	ALLLED_OFF();
	LED_RED_ON();
#ifdef ddebug
Send_String_Poll("led red on\n");
#endif
	//delay();
}

