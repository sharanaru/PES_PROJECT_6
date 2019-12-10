/*
 * buffer_functions.c
 *
 *  Created on: 11-Nov-2019
 *      Author: Hp
 */
#include <buffer_functions.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fsl_debug_console.h>
uint8_t rw;
enum Error_status result;


/* Function to create buffer */
// Pass a storage buffer and size
enum Error_status create_buffer(uint16_t *buffer_t,user_n *user_t,uint16_t *size)
{
	if(*size>0)
	{
	buffer_t = (uint16_t *)malloc(*size *sizeof(uint16_t));
		if(buffer_t!=NULL)
			{
				result = SUCCESS;
				user_t->buffer = buffer_t;
				user_t->head = buffer_t;
				user_t->tail = buffer_t;
				user_t->maxlen = *size;
			//	PRINTF("\n\rmaxlen = %d\n\r",user_t->maxlen);
				//PRINTF("user_t.head = %p\n\r",user_t->head);
			//	PRINTF("\n\rlast value = %p\n\r",(user_t->buffer + (user_t->maxlen-1)));
			}
		else
			{
			PRINTF("\n\rBUFFER NOT ALLOCATED\n\r");
			result = FAILURE;
			}
	}
	else
	{
		PRINTF("\n\rEnter size greater than 0\n\r");
		result = FAILURE;
	}
	//	user_t.maxlen = size;
	//return (int)buffer_t;
	return result;
}

/* Reallocate Buffer */
// Function called when original buffer becomes full

enum Error_status overflow_handler(uint16_t *buffer_old,user_n *user_t,uint16_t size)
{
	size = size + 10;
	if(size>0)
	{
	//uint16_t *buffer_rt = (uint16_t *)malloc(*size*sizeof(uint16_t));
	uint16_t *buffer_rt = (uint16_t *)realloc(buffer_old,(unsigned int)size*sizeof(uint8_t));
//	PRINTF("\n\rrealloc address = %p\n\r",buffer_rt);
			//(user_t->buffer,*size*sizeof(uint16_t));
		if(buffer_rt!=NULL)
				{
				result = SUCCESS;
				user_t->buffer = buffer_rt;
				// Head+last position
		//		PRINTF("\n\rhead_count = %d\n\r",user_t->head_count);
				user_t->head = buffer_rt;
				user_t->head = user_t->head + user_t->head_count;
				user_t->tail = buffer_rt;
				user_t->tail = user_t->tail +  user_t->tail_count;
				user_t->maxlen = size;
			//	PRINTF("\n\rmaxlen = %d\n\r",user_t->maxlen);
			//	PRINTF("user_t.head = %p\n\r",user_t->head);
			//	PRINTF("\n\rlast value = %p\n\r",(user_t->buffer + (user_t->maxlen-1)));
			}
		else
			{
			PRINTF("\n\rBUFFER NOT REALLOCATED\n\r");
			result = FAILURE;
			}
	}
	else
	{
		PRINTF("\n\rEnter size greater than 0\n\r");
		result = FAILURE;
	}
	//	user_t.maxlen = size;
	//return (int)buffer_t;
	return result;
}




/*Buffer full Function*/
// Checks if buffer is full

bool buffer_full(user_n *user_t)
{

	return user_t->full;
}



/*Buffer Empty Function*/
// Checks if buffer is empty
// Returns a false when empty

bool buffer_empty(user_n *user_t)
{
	//PRINTF("\n\rhead = %x\n\r",user_t->head);
	//PRINTF("\n\rtail = %x\n\r",user_t->tail);
	//PRINTF("\n\rfull = %d\n\r",user_t->full);
	//PRINTF("\n\rEMPTY = %d\n\r",((!user_t->full) && (user_t->head==user_t->tail)));
	return ((!user_t->full) && (user_t->head==user_t->tail));
}

/* Dummy write to buffer (using SCANF) */
// Write to the head address and advance the head location
// If the buffer is full we need to advance our tail pointer as well as head
// increment head while writing


/* Reset Buffer */
// Set the structure elements to 0
// put head==tail
void buffer_reset(user_n *user_t)
{
	user_t->head = user_t->buffer;
	user_t->tail = user_t->buffer;
	user_t->full = false;

}

/* Destroy Buffer */
// call free on the buffer address
void buffer_destroy(user_n *user_t)
{
	free(user_t->buffer);

}


/* Advance Pointer */
// PASS TAIL and HEAD
void advance_pointer(user_n *user_t)
{
	if(user_t->full)
	{
		user_t->tail++;
		user_t->tail_count++;
		if(user_t->tail == ((user_t->buffer+user_t->maxlen)))
		{
			user_t->tail = user_t->buffer;
			//user_t->tail_count = 0;
		}
	}

	//user_t->head = (user_t->head+1);
	user_t->head++;
	user_t->head_count++;
	if(user_t->head == ((user_t->buffer+user_t->maxlen)))
	{
	//	PRINTF("\n\rRollover head = %p\n\r",user_t->head);
		user_t->head = user_t->buffer;
		//user_t->head_count = 0;
	}

	user_t->full=(user_t->head==user_t->tail);
}

/* Retreat Pointer */
// PASS TAIL
void retreat_pointer(user_n *user_t)
{
	user_t->full = false;
	user_t->tail++;
	user_t->tail_count++;
			if(user_t->tail == ((user_t->buffer+user_t->maxlen)))
			{
				user_t->tail = user_t->buffer;
				//user_t->tail_count = 0;
			}
}

/* Size function */
// PASS HEAD and TAIL
//int cbuf_size(user_n *user_t)
//{
//	if(user_t->head>=user_t->tail)
//	{
//
//		user_t->count = user_t->head - user_t->tail;
//	}
//	else
//	{
//		user_t->count = ((user_t->maxlen + user_t->head)-user_t->tail);
//	}
//
//	return user_t->count;
//}

int character_count(user_n *user_t)
{
	// rw == 1 while writing
	// rw == 0 while reading
	if(rw == 1)
	{
		user_t->count++;
	}
	else if(rw == 0)
	{
		user_t->count--;

	}
//	PRINTF("\n\rcount = %d\n\r",user_t->count);
	return user_t->count;
}




/* Read from buffer */
// Increment tail while writing

int buffer_read(user_n *user_t, uint8_t readdata)
{

	readdata = *(user_t->tail);
	retreat_pointer(user_t);
	//PRINTF("\n\rreaddata = %d, tail = %d\n\r",readdata,*(user_t->tail));
	//PRINTF("\n\rREAD ADDRESS = %p\n\r",user_t->tail);
	rw = 0;
	character_count(user_t);
	if(buffer_empty(user_t))
	{

		//PRINTF("\n\rBUFFER IS EMPTY!\n\r");
		user_t->tail = user_t->buffer;
	}
	return readdata;
}

/* Write to buffer */
// Write to the head address and advance the head location
// If the buffer is full we need to advance our tail pointer as well as head
// increment head while writing
void buffer_write(uint16_t *pointer,user_n *user_t, uint32_t writedata, uint16_t size,uint8_t *flag)
{

		buffer_full(user_t);
		*(user_t->head) = writedata;
		//PRINTF("\n\r data at head = %d\n\r",*(user_t->head));
		//PRINTF("\n\r head address = %p\n\r",user_t->head);
		advance_pointer(user_t);
		//cbuf_size(user_t);
		rw = 1;
		character_count(user_t);
		if(buffer_full(user_t))
			{
				user_t->full = false;
			//	PRINTF("\n\rBUFFER IS FULL!\n\r");
				*flag = 1;
				// Reallocate function
				//overflow_handler(pointer,user_t,size);
				//buffer_destroy(user_t);


			}
		if(buffer_empty(user_t))
			{
		//		PRINTF("\n\rBUFFER IS EMPTY!\n\r");
			}

}





