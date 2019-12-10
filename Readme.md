# Project 6 PES - Readme

Team Members: 
Sharan Arumugam
Abhijeet Srivastava

# Execution Instructions
+ Using custom build configuration this time, to compile for different modes. As always ,please run clean before building a new configuration. 
+ 2 Configurations : Debug, Normal
+ Program 1 exists in a seperate folder named 'Program1 respectively in the repo' -Program only generates a sin wave. Program logic has been used in the main 2nd program

## Steps for building application

**+ COMPILER COMMANDS**
+ *For building Debug config :* 
arm-none-eabi-gcc -D__REDLIB__ -DCPU_MKL25Z128VLK4 -DCPU_MKL25Z128VLK4_cm0plus -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -Dddebug -I../board -I../source -I../ -I../drivers -I../CMSIS -I../utilities -I../startup -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"source/t1.d" -MT"source/t1.o" -MT"source/t1.d" -o "source/t1.o" "../source/t1.c"
+ *For building Normal config :* 
arm-none-eabi-gcc -D__REDLIB__ -DCPU_MKL25Z128VLK4 -DCPU_MKL25Z128VLK4_cm0plus -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG  -I../board -I../source -I../ -I../drivers -I../CMSIS -I../utilities -I../startup -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"source/t1.d" -MT"source/t1.o" -MT"source/t1.d" -o "source/t1.o" "../source/t1.c"



# Files in Repo
+ MCU XPRESSO Project Directory
     + Board Folder: Contains regular board files plus
	    + Logging.h -Logger functions
		+ led_board.h/.c These contain the LED functions
	    + buffer_functions.h/.c Contains the buffer control functions
	                      the unit tests  
     + Source Folder - Contains main routine 	
     + Default folders with no change -CMSIS,startup,drivers,utilities
     + Debug - Contains .o files after compilation and also contains .axf binary and .exe files
+ Readme 
+ Screenshots of Sine waves



## Issues faced

+ DMA transfer wasnt receiving buffer pointer properly, had to create a different pointer to point to buffer seperately 
+ Creating DAC value analysis task in another task, couldnt make it resume from  interrupt.
+ Got MUTEX working for led control after repeated tries.   
+  
## References
+ Slack channel for clearing up questions 
+ KL25z examples from the SDK- Examples for interfacing DAC,ADC,DMA,MUTEX sample
+ https://www.nxp.com/docs/en/reference-manual/KL25P80M48SF0RM.pdf
+ https://www.geeksforgeeks.org/mathematics-mean-variance-and-standard-deviation/
