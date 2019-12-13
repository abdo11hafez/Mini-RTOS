#ifndef TASK_H
#define TASK_H


#define TASK_STACK_SIZE 1024
#define TASK_PRIORITY_LEVELS 16
	
#include <stdint.h>
#include "list.h"

typedef struct
{
	uint32_t pStack;       				 
	uint32_t priority;             
	uint32_t OriginalPriority;	
	ListItem_t StatusListItem;     /**< ReadyList or TimerWaitingList */
	ListItem_t ObjectListItem;     /**< MutexWaitingList or SemaphoreWaitingList */
} Task_t;

typedef struct
{
	uint32_t stack[TASK_STACK_SIZE];  /**< Thread stack */
} Stack_t;


void Init_Lists(void);
void AddTaskToReadyList(Task_t * pTask);
void Create_Task(Task_t * Task, Stack_t * Stack,uint32_t priority, void * Function);
void UpdateRunningTask(void);
Task_t * GetRunningTask(void);
void DelayRunningTask(uint32_t WaitTime);
void RefreshTimerList(void);
void InheritTaskPrioroty(Task_t *pTask);
void DisinheritTaskPriority(void);
#endif
