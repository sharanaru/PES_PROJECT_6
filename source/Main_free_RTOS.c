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

 */
//Abhijeet and SHARAN -PES Project 6

//#define PRINTF printf
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "math.h"
#include "Led.h"
#include "semphr.h"
#include "logging.h"
/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_dac.h"
#include "fsl_adc16.h"
#include "board.h"
#include "fsl_dma.h"
#include "fsl_dmamux.h"
#include "buffer_functions.h"
#include "math.h"
#include "stdint.h"

#include "pin_mux.h"
/*******************************************************************************
 * Global Variables
 ******************************************************************************/
uint16_t destAddr[64]={0};
uint8_t task_counter = 0;
TaskHandle_t *const DAC_t = NULL;
TaskHandle_t *const ADC_t = NULL;
TaskHandle_t *const DSP_A = NULL;
uint16_t SineLUT[51]={0};
uint8_t DSP_FLAG = 0;
QueueHandle_t sampleholder;

/* Task priorities. */
#define Highest_task_Priority (configMAX_PRIORITIES - 1)
#define High_task_PRIORITY (configMAX_PRIORITIES - 2)
#define Common_task_Priority (configMAX_PRIORITIES - 3)
#define Lowest_task_Priority (configMAX_PRIORITIES - 4)

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define DEMO_DAC_BASEADDR DAC0
#define DEMO_ADC16_BASE ADC0
#define DEMO_ADC16_CHANNEL_GROUP 0U
#define DEMO_ADC16_USER_CHANNEL 0U /*PTE20, ADC0_SE0 */

#define DMA_CHANNEL 0
#define DMA_SOURCE 63

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
dma_transfer_config_t transferConfig;
dma_handle_t g_DMA_Handle;
static void ADC_Task(void *pvParameters);
static void DAC_Task(void *pvParameters);
static void DSP_Analysis(void *pvParamaters);

/*******************************************************************************
 * Delay defination
 ******************************************************************************/
const TickType_t xDelay100ms = pdMS_TO_TICKS( 100 );
const TickType_t xDelay1ms = pdMS_TO_TICKS( 1 );
const TickType_t xDelay5ms = pdMS_TO_TICKS( 5 );

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Application entry point.
 */
//led mutex
SemaphoreHandle_t xMutex;
int main(void)
{
	/* Init board hardware. */
	BOARD_InitPins();
	BOARD_BootClockRUN();
	BOARD_InitDebugConsole();
	int index;
	for(index = 0;index<51;index++)
	{
		SineLUT[index] = ( ( ( sin(index * (6.28/50)))+2)*4096/3.3 );

	}
	sampleholder= xQueueCreate(64,sizeof(uint16_t));
	if( sampleholder == NULL )
	{
		PRINTF("\n\r Queue not created");
		return -1;
		/* Queue was not created and must not be used. */
	}
	//initilaises led
	LED_Initialise();
#ifdef dedebug
	PRINTF("MODE is DEBUG\n\r");
	ledstatus("\n\rLEDS initialised");
#endif
	xMutex=xSemaphoreCreateMutex();
	//Create DAC Task
	xTaskCreate(DAC_Task, "DAC_Task", configMINIMAL_STACK_SIZE + 50, NULL,High_task_PRIORITY,DAC_t);
	//ADC task
	xTaskCreate(ADC_Task, "ADC_Task", configMINIMAL_STACK_SIZE + 800, NULL, Common_task_Priority,ADC_t);


	vTaskStartScheduler();

	for (;;)
		;
}

// SW_Timer call back function

volatile bool g_Transfer_Done;
int k=0;
void DMA_Callback(dma_handle_t *handle, void *param)
{
	g_Transfer_Done=true;
	//DSP_FLAG = 1;

}

static void ADC_Task(void *pvParameters)
{

	logtask("ADC TASK ON\n\r");

	TickType_t Lastwakeuptime;
	Lastwakeuptime = xTaskGetTickCount();

	TickType_t DMA_Start;
	TickType_t DMA_Stop;
	TickType_t timestamp_start;
	TickType_t timestamp_end;
	TickType_t end;

	uint16_t space = 64;
	uint8_t DMA_FLAG = 0;
	uint8_t DSP_FLAG = 0;
	uint32_t i = 0;
	g_Transfer_Done = false;
	uint16_t *buffer_p = NULL;
	//uint16_t destAddr[64]={0};
	user_n user = {NULL,NULL,NULL,0,0,0};

	//SwTimerHandle = xTimerCreate("timer_5", xDelay5ms, pdTRUE, 0, Timer_pt5second);

	create_buffer(buffer_p,&user, &space);
	uint16_t *srcpointer=user.buffer;

	adc16_config_t adc16ConfigStruct;
	adc16_channel_config_t adc16ChannelConfigStruct;

	//	    BOARD_InitPins();
	//	    BOARD_BootClockRUN();
	//	    BOARD_InitDebugConsole();

	ADC16_GetDefaultConfig(&adc16ConfigStruct);
#ifdef BOARD_ADC_USE_ALT_VREF
	adc16ConfigStruct.referenceVoltageSource = kADC16_ReferenceVoltageSourceValt;
#endif
	ADC16_Init(DEMO_ADC16_BASE, &adc16ConfigStruct);
	ADC16_EnableHardwareTrigger(DEMO_ADC16_BASE, false); /* Make sure the software trigger is used. */
#if defined(FSL_FEATURE_ADC16_HAS_CALIBRATION) && FSL_FEATURE_ADC16_HAS_CALIBRATION
	if (kStatus_Success == ADC16_DoAutoCalibration(DEMO_ADC16_BASE))
	{
		PRINTF("ADC16_DoAutoCalibration() Done.\r\n");
	}
	else
	{
		PRINTF("ADC16_DoAutoCalibration() Failed.\r\n");
	}
#endif /* FSL_FEATURE_ADC16_HAS_CALIBRATION */

	adc16ChannelConfigStruct.channelNumber = DEMO_ADC16_USER_CHANNEL;
	adc16ChannelConfigStruct.enableInterruptOnConversionCompleted = false;

#if defined(FSL_FEATURE_ADC16_HAS_DIFF_MODE) && FSL_FEATURE_ADC16_HAS_DIFF_MODE
	adc16ChannelConfigStruct.enableDifferentialConversion = false;
#endif /* FSL_FEATURE_ADC16_HAS_DIFF_MODE */

	uint16_t loader=0;
	for(;;)
	{


		ADC16_SetChannelConfig(DEMO_ADC16_BASE, DEMO_ADC16_CHANNEL_GROUP, &adc16ChannelConfigStruct);
		while (0U == (kADC16_ChannelConversionDoneFlag &
				ADC16_GetChannelStatusFlags(DEMO_ADC16_BASE, DEMO_ADC16_CHANNEL_GROUP)))
		{
		}
		loader= ADC16_GetChannelConversionValue(DEMO_ADC16_BASE, DEMO_ADC16_CHANNEL_GROUP);
#ifdef dedebug
		PRINTF("ADC Value: %d\r\n", loader);
#endif
		buffer_write(buffer_p, &user,ADC16_GetChannelConversionValue(DEMO_ADC16_BASE, DEMO_ADC16_CHANNEL_GROUP), space, &DMA_FLAG);
		//if(xQueueSend(sampleholder,&loader,1)==pdFAIL)
		//{
		//DMA_FLAG=1;
		//}

		if(1==DMA_FLAG)
		{

			timestamp_start = xTaskGetTickCount();
			DMA_Start = xTaskGetTickCount();

			PRINTF("\n\rDMA START %d:%d seconds \n\r",(DMA_Start/1000),timestamp_start);

			DMA_Stop = DMA_Start + (500*xDelay1ms);

			LED_GREEN_OFF();

			xSemaphoreTake(xMutex, portMAX_DELAY);


			while(DMA_Start != DMA_Stop)
			{
				DMA_Start = xTaskGetTickCount();
				GPIOB->PDDR&=0x00;
				LED_GREEN_OFF();
				LED_BLUE_ON();

			}

			LED_BLUE_OFF();
			xSemaphoreGive(xMutex);


			GPIOB->PDDR|=0xFF;
			LED_GREEN_INIT(1);
			LED_BLUE_INIT(1);


			DMA_FLAG = 0;
			PRINTF("FLAG SET\n\r");

			//DMA CODE HERE

			PRINTF("DMA memory to memory transfer example begin.\r\n\r\n");
			PRINTF("Destination Buffer:\r\n");
#ifdef dedebug
			for (i = 0; i < 64 ; i++)
			{
				PRINTF("%d\t\r", destAddr[i]);
			}
#endif
			/* Configure DMAMUX */
			DMAMUX_Init(DMAMUX0);
			DMAMUX_SetSource(DMAMUX0, DMA_CHANNEL, DMA_SOURCE);
			DMAMUX_EnableChannel(DMAMUX0, DMA_CHANNEL);
			/* Configure DMA one shot transfer */
			DMA_Init(DMA0);
			dma_handle_t *ptrHandle=&g_DMA_Handle;
			DMA_CreateHandle(&g_DMA_Handle, DMA0, DMA_CHANNEL);
			DMA_SetCallback(&g_DMA_Handle, DMA_Callback, NULL);
			DMA_PrepareTransfer(&transferConfig, srcpointer, sizeof(uint16_t), destAddr, sizeof(uint16_t), 128,
					kDMA_MemoryToMemory);
			DMA_SubmitTransfer(&g_DMA_Handle, &transferConfig, kDMA_EnableInterrupt);
			DMA_StartTransfer(&g_DMA_Handle);


			while(ptrHandle->base->DMA[ptrHandle->channel].DSR_BCR & DMA_DSR_BCR_BSY_MASK)
				;
			/* Wait for DMA transfer finish */
			//		        			    while (g_Transfer_Done != true)
			//		        			    {
			//		        			    }
			/* Print destination buffer */
			PRINTF("\r\n\r\nDMA memory to memory transfer example finish.\r\n\r\n");
#ifdef dedebug

			PRINTF("Destination Buffer:\r\n");
#endif
			for (i = 0; i < 64; i++)
			{
#ifdef dedebug
				PRINTF("%d \n\r", destAddr[i]);
#endif
				if(63==i)
				{
					DSP_FLAG = 1;
				}
			}

			//DMA CODE HERE
			timestamp_end = xTaskGetTickCount();
			end = xTaskGetTickCount();
			PRINTF("DMA End time = %d:%d\n\r",(timestamp_end/1000),end);

		}

		if(DSP_FLAG == 1)
		{
			DSP_FLAG = 0;
			xTaskCreate(DSP_Analysis, "DSP_Task", configMINIMAL_STACK_SIZE+20,NULL,Highest_task_Priority, DSP_A);
			//xTaskResumeFromISR(DSP_A);
		}
		vTaskDelayUntil(&Lastwakeuptime, xDelay100ms);
	}
	//		for(;;)
	//		{ }

}




static void DAC_Task(void *pvParameters)
{
	logtask("DAC_TASK");
	uint8_t index = 0;
	//	const uint32_t SineLUT[] = {
	//			2631,2780,2929,3077,3202,3326,3437,3524,3599,3661,3710,3712,3713,3710,
	//			3661,3599,3524,3437,3326,3202,3077,2929,2780,2631,2482,2321,2172,2023,
	//			1874,1750,1626,1526,1427,1353,1291,1253,1241,1240,1241,1253,1291,1353,
	//			1427,1526,1626,1750,1874,2023,2172,2321,2482};

	TickType_t Lastwakeuptime;
	Lastwakeuptime = xTaskGetTickCount();

	dac_config_t dacConfigStruct;


	DAC_GetDefaultConfig(&dacConfigStruct);
	DAC_Init(DEMO_DAC_BASEADDR, &dacConfigStruct);
	DAC_Enable(DEMO_DAC_BASEADDR, true);             /* Enable output. */
	DAC_SetBufferReadPointer(DEMO_DAC_BASEADDR, 0U); /* Make sure the read pointer to the start. */


	for(;;)
	{
		//	PRINTF("DAC Example\r\n");

		// Toggle GREEN LED every time DAC runs
		xSemaphoreTake(xMutex, portMAX_DELAY);//takes mutex semaphore
#ifdef dedebug
		ledstatus("\n\rGreen led Toggled\n\r");
#endif
		LED_GREEN_TOGGLE();

		xSemaphoreGive(xMutex);
		DAC_SetBufferValue(DEMO_DAC_BASEADDR, 0U, SineLUT[index]);
		index++;
		if(51==index)
		{
			index=0;
		}

		vTaskDelayUntil(&Lastwakeuptime, xDelay100ms);
	}

}

static void DSP_Analysis(void *pvParameters)
{

	logtask("DSP_Analysis\n\r");
	uint8_t DSP_Index = 0;
	float MAX = 0;
	float MIN = 0;
	float AVG = 0;
	float varn = 0;
	float stddev = 0;
	task_counter++;
	for(;;)
	{
		MAX = destAddr[0];
		MIN = destAddr[0];

		for(DSP_Index = 0;DSP_Index<64;DSP_Index++)
		{
			//PRINTF("DSP_Analysis Buffer = %d  \n\r",destAddr[DSP_Index]);
			AVG = AVG + destAddr[DSP_Index];
			if(destAddr[DSP_Index]>MAX)
			{
				MAX = destAddr[DSP_Index];
			}
			if(destAddr[DSP_Index]<MIN)
			{
				MIN = destAddr[DSP_Index];
			}

		}
		AVG = AVG/64;

		//calculate variance
		for(DSP_Index = 0;DSP_Index<64;DSP_Index++)
		{

			varn = varn + powf( ( ((float)destAddr[DSP_Index]) - AVG) , 2 );

		}

		varn = varn/64;
		stddev = sqrtf(varn);

		PRINTF("\n\rAVG value = %f\n\r",AVG);
		PRINTF("\n\rMax value = %f\n\r",MAX);
		PRINTF("\n\rMin value = %f\n\r",MIN);
		PRINTF("\n\rStandard Deviation = %f %\n\r",stddev);
		PRINTF("\n\rTask counter = %d %\n\r",task_counter);

		if(task_counter==5)
		{
			PRINTF("Tasks done 5 times\n\r");
			vTaskSuspendAll();
		}
		vTaskSuspend(NULL);
	}
}

