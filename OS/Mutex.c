#include "Mutex.h"
#include "task.h"
#include "M4MemMap.h"

void CreateMutex(Mutex_t *pMutex, uint32_t initialValue)
{
	if((pMutex==NULL)||((initialValue != 0)&&(initialValue != 1)))
	{
		__asm { CPSID i }         /* Disable Interrupts */
		while (1)
		{ 			
		}
	}
	List_Init(&pMutex->waitingList);
	pMutex->mutexValue = initialValue;
	pMutex->pTask = NULL;			
}

uint32_t MutexLock(Mutex_t * pMutex, int32_t WaitTicks)
{
	uint32_t returnStatus = 0;
	Task_t * pTaskRunning;
	uint32_t terminate = 0;
	uint32_t MutexTempValue = 2;
	uint32_t Error = 2;
	if((pMutex==NULL)||(WaitTicks <-1))
	{
		__asm { CPSID i }         /* Disable Interrupts */
		while (1)
		{ 			
		}
	}
	while(1 != terminate)
	{
		__asm 
		{
			LDREX   MutexTempValue, [&(pMutex->mutexValue)]
		}
		if(MutexTempValue == 1)
		{
			__asm
			{
				STREX  Error ,0,[&pMutex->mutexValue]
			}
			/* Mutex is free, lock it */
			if(Error == 0)
			{
				__asm
				{
					DMB 
				}
			pMutex->pTask = GetRunningTask();
			returnStatus = 1;
			terminate = 1;
			}
			else
			{
				/* try again */
			}
		}
		else
		{
			/* Mutex is locked */
			terminate = 1;
		}
	}
	if((WaitTicks != 0) && (returnStatus != 1))
	{
		pTaskRunning = GetRunningTask();
		List_Remove(&pTaskRunning->StatusListItem);
		ListInsertAtItemValue(&pMutex->waitingList, &pTaskRunning->ObjectListItem);
		SCB_INTCTRL_REG |= 1<<28 ;
		if(WaitTicks > 0)
		{
			DelayRunningTask(WaitTicks);
		}
		else
		{
			/* Infinite Waiting */
		}
		InheritTaskPrioroty(pMutex->pTask);
		returnStatus = 2;
		
	}
	else
	{
		/* No Waiting Required */
	}
	return returnStatus;
}


void MutexRelease(Mutex_t * pMutex)
{
	Task_t * Task;
	if(pMutex==NULL)
	{
		__asm { CPSID i }         /* Disable Interrupts */
		while (1)
		{ 			
		}
	}
	__asm
	{
		DMB 
	}
	pMutex->mutexValue = 1;     /* Release the mutex */
	/* Get highest priority Waiting Task at the Mutex WaitingList*/
	if(pMutex->waitingList.numOfItems > 0)
	{
		Task = pMutex->waitingList.listEnd.pNext->pTask;
		if (Task==NULL)
		{
			__asm { CPSID i }         /* Disable Interrupts */
			while (1)
			{ 			
			}
		}
		List_Remove(&Task->ObjectListItem);
		if(Task->StatusListItem.pList != NULL)
		{
			List_Remove(&Task->StatusListItem);
		}
		AddTaskToReadyList(Task);
	}
	else
	{
		/* No Tasks are waiting*/
	}
	DisinheritTaskPriority();
	pMutex->pTask = NULL;
}
