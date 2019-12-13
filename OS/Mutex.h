#ifndef MUTEX_H
#define MUTEX_H


#include "list.h"
#include "Task.h"
typedef struct
{
	uint32_t mutexValue;        
	List_t waitingList;  
	Task_t * pTask;
} Mutex_t;


void CreateMutex(Mutex_t *pMutex, uint32_t initialValue);
uint32_t MutexLock(Mutex_t * pMutex, int32_t WaitTicks);
void MutexRelease(Mutex_t * pMutex);
#endif

