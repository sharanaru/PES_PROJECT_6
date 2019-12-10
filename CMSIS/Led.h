/*
 * Led.h
 *
 *  Created on: 10-Dec-2019
 *      Author: Hp
 */

#ifndef LED_H_
#define LED_H_



#endif /* LED_H_ */

#include<stdio.h>
#include<stdint.h>

#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "MKL25Z4.h"

//#include "uarts.h"
//all functions dependent on platform
void LED_Initialise(); //depending on platform led initialises or does nothing
void ALLLED_OFF();//switches all led off
void LED_PASS();//switches on green led and all others off
void LED_PROCESS();//switches on blue led and others off
void LED_FAIL();//switches on red led and others off
void delay();//delay to create halt in order to observe led status
