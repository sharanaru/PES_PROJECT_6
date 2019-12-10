/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Standard includes. */
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_dac.h"
#include "board.h"
#include "math.h"

#include "pin_mux.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* The software timer period. */
#define SW_TIMER_PERIOD_MS (100 / portTICK_PERIOD_MS)
#define DEMO_DAC_BASEADDR DAC0
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* The callback function. */
static void SwTimerCallback(TimerHandle_t xTimer);

uint32_t value = 0;
uint32_t buffer[64];
uint8_t index = 0;

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
    TimerHandle_t SwTimerHandle = NULL;

    /* Init board hardware. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    SystemCoreClockUpdate();
    /* Create the software timer. */
    SwTimerHandle = xTimerCreate("SwTimer",          /* Text name. */
                                 SW_TIMER_PERIOD_MS, /* Timer period. */
                                 pdTRUE,             /* Enable auto reload. */
                                 0,                  /* ID is not used. */
                                 SwTimerCallback);   /* The callback function. */
   for(index = 0;index<50;index++)
   {
	   buffer[index] = ( ( ( sin(index * (6.28/50)))+2)*4096/3.3 );
	   PRINTF("SINE = %d\n\r",buffer[index]);
   }

// DAC

   dac_config_t dacConfigStruct;

  //    BOARD_InitPins();
  //    BOARD_BootClockRUN();
  //    BOARD_InitDebugConsole();

      PRINTF("\r\nDAC basic Example.\n\r");

      /* Configure the DAC. */
      /*
       * dacConfigStruct.referenceVoltageSource = kDAC_ReferenceVoltageSourceVref2;
       * dacConfigStruct.enableLowPowerMode = false;
       */
      DAC_GetDefaultConfig(&dacConfigStruct);
      DAC_Init(DEMO_DAC_BASEADDR, &dacConfigStruct);
      DAC_Enable(DEMO_DAC_BASEADDR, true);             /* Enable output. */
      DAC_SetBufferReadPointer(DEMO_DAC_BASEADDR, 0U); /* Make sure the read pointer to the start. */
                                                       /*
                                                       * The buffer is not enabled, so the read pointer can not move automatically. However, the buffer's read pointer
                                                       * and items can be written manually by user.
                                                       */
// DAC

    /* Start timer. */
    xTimerStart(SwTimerHandle, 0);

    /* Start scheduling. */
    vTaskStartScheduler();
    for (;;)
        ;
}

/*!
 * @brief Software timer callback.
 */
static void SwTimerCallback(TimerHandle_t xTimer)
{
    PRINTF("Tick.\r\n");
    DAC_SetBufferValue(DEMO_DAC_BASEADDR, 0U, buffer[index]);
    index++;
    if(51==index)
    {
    	index=0;
    }
}
