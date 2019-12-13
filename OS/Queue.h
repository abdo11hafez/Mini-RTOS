#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>
#include "List.h"

typedef struct
{
	int8_t * pFront;         /**< Start location */
	int8_t * pEnd;           /**< Last location */
	int8_t * pReadIndex;      /**< Read location */
	int8_t * pWriteIndex;    /**< Write location */
	uint32_t QueueLength;    /**< Length of the mailbox buffer */
	uint32_t MessageSize;    /**< Mailbox message size in bytes */
	uint32_t MessagesCount;  /**< Current number of messages inside the buffer */
	List_t WaitingList;      /**< Waiting list */
} Queue_t;


void QueueCreate(Queue_t * pQueue, void * pBuffer,uint32_t QueueLength, uint32_t MessageSize);
uint8_t QueueWrite(Queue_t * pQueue, int32_t WaitTicks,const void * const pMessage);
uint8_t QueueRead (Queue_t * pQueue, int32_t WaitTicks,void * const pMessage);



#endif

