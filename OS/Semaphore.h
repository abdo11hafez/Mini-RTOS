#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "list.h"

typedef struct
{
	uint32_t SemaphoreValue;        
	List_t WaitingList;            
} Semaphore_t;

void SemaphoreCreate(Semaphore_t * pSemaphore, uint32_t initialValue);
uint32_t SemaphoreTake(Semaphore_t * pSemaphore, int32_t WaitFlag);
void SemaphoreGive(Semaphore_t * pSemaphore);

#endif

