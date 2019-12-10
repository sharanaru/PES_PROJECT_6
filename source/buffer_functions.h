/*
 * buffer_functions.h
 *
 *  Created on: 11-Nov-2019
 *      Author: Hp
 */

#ifndef BUFFER_FUNCTIONS_H_
#define BUFFER_FUNCTIONS_H_
#endif /* BUFFER_FUNCTIONS_H_ */

#include <stdbool.h>
#include <stdint.h>
//#include "systick.h"

// Enum for circular buffer errors
enum Error_status{SUCCESS, FAILURE};


/* Structure for circular buffer */

typedef struct user_helper
	{
			uint16_t *buffer; //Buffer pointer const uint8_t * means the value pointed t can't be changed, uint8_t const * means the value can be changed but not the pointer
			uint16_t *head;	// Incremented when write occurs
			uint16_t *tail;	// incremented when read occurs
			uint16_t maxlen;	// Length of buffer
			int count;	//
			int head_count;
			int tail_count;
			bool full;
	} user_n;


//	void create_buffer(user_n *user_t,uint16_t size);
//	bool buffer_full(user_n *user_t);
//	bool buffer_empty(user_n *user_t);
//	void buffer_reset(user_n *user_t);
//	void buffer_destroy(user_n *user_t);
//	void advance_pointer(user_n *user_t);
//	void retreat_pointer(user_n *user_t);
//	int cbuf_size(user_n *user_t);
//	int buffer_read(user_n *user_t, uint8_t *readdata);
//	void buffer_write(user_n *user_t, uint8_t *writedata);

	enum Error_status create_buffer(uint16_t *buffer_t,user_n *user_t,uint16_t *size);
	bool buffer_full(user_n *user_t);
	bool buffer_empty(user_n *user_t);
	void buffer_reset(user_n *user_t);
	void buffer_destroy(user_n *user_t);
	void advance_pointer(user_n *user_t);
	void retreat_pointer(user_n *user_t);
	int character_count(user_n *user_t);
	int buffer_read(user_n *user_t, uint8_t readdata);
	void buffer_write(uint16_t *pointer,user_n *user_t, uint32_t writedata, uint16_t size,uint8_t *flag);
	enum Error_status overflow_handler(uint16_t *buffer_rt,user_n *user_t,uint16_t size);









