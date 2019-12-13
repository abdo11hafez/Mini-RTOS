#ifndef OS_H
#define OS_H

#include "task.h"
#include "Semaphore.h"
#include "Mutex.h"
#include "Queue.h"

#define TASK_STACK_SIZE 1024
#define TASK_PRIORITY_LEVELS 16


void OS_Init(void);
void OS_Start_Scheduler(void);
uint32_t OS_GetSchedulerStatus(void);
void Start_Scheduler(void);
void OS_TaskCreate(Task_t * Task, Stack_t * Stack,uint32_t priority, void * Function);
void OS_TaskDelay(uint32_t Ticks);
void OS_SemaphoreCreate(Semaphore_t * pSemaphore, uint32_t InitialValue);
void OS_SemaphoreTake(Semaphore_t * pSemaphore, int32_t WaitTicks);
void OS_SemaphoreGive(Semaphore_t * pSemaphore);
void OS_MutexCreate(Mutex_t * pMutex,uint32_t initialValue);
void OS_MutexLock(Mutex_t * pMutex,int32_t WaitTicks);
void OS_MutexRelease(Mutex_t * pMutex);
void OS_QueueCreate(Queue_t * pQueue, void * pBuffer,uint32_t qQueueLength, uint32_t MessageSize);
void OS_QueueWrite(Queue_t * pQueue, int32_t WaitTicks,const void * const pMessage);
void OS_QueueRead(Queue_t * pQueue, int32_t WaitTicks,void * const pMessage);
#endif

