#include "Semaphore.h"
#include "task.h"
#include "M4MemMap.h"

void SemaphoreCreate(Semaphore_t * pSemaphore, uint32_t initialValue)
{
	if (pSemaphore == NULL)
	{
		__asm { cpsid i } /* Disable Interrupts */
		while (1)
		{ 			
		}
	}
	List_Init(&pSemaphore->WaitingList);
	pSemaphore->SemaphoreValue = initialValue;
}


uint32_t SemaphoreTake(Semaphore_t * pSemaphore,int32_t WaitTicks)
{
	uint32_t returnStatus = 0;
	Task_t * pTaskRunning = NULL;
	uint32_t semaphoreValueTemp = 0;
	uint32_t terminate = 0;
	uint32_t Error = 2;

	if ((pSemaphore == NULL)||(WaitTicks < -1))
	{
		__asm { cpsid i } /* Disable Interrupts */
		while (1)
		{ 			
		}
	}
	while(terminate != 1)
	{
		/* Read the semaphore counter value */
		__asm 
		{
		LDREX   semaphoreValueTemp, [&(pSemaphore->SemaphoreValue)]
		}
		if(semaphoreValueTemp > 0)
		{
			__asm
			{
				STREX  Error ,(semaphoreValueTemp - 1),[&pSemaphore->SemaphoreValue]
			}
			if(Error == 0)
			{
				__asm
				{
					DMB 
				}
			returnStatus = 1;
			terminate = 1;
			}
			else
			{
			/* atomic operation interrupted ... try again */
			}
		}	
		else
		{
		/* Semaphore is not available */
		terminate = 1;
		}
	}
	if((WaitTicks != 0) && (returnStatus != 1))
	{
		pTaskRunning = GetRunningTask();
		List_Remove(&pTaskRunning->StatusListItem);
		ListInsertAtItemValue(&pSemaphore->WaitingList, &pTaskRunning->ObjectListItem);
		SCB_INTCTRL_REG |= 1<<28 ;
		if(WaitTicks > 0)
		{
			DelayRunningTask(WaitTicks);
		}
		else
		{
			/* Infinite Wait*/
		}
		returnStatus = 2;
	}
	else
	{
		/* No Waiting Required */
	}
return returnStatus;
}


void SemaphoreGive(Semaphore_t * pSemaphore)
{
	Task_t * pTaskToUnblock;
	uint32_t SemaphoreTempValue = 0;
	uint32_t terminate = 0;
	uint32_t Error = 2;

	if (pSemaphore == NULL)
	{
		__asm { cpsid i } /* Disable Interrupts */
		while (1)
		{ 			
		}
	}
	while(1 != terminate)
	{
		/* Get the semaphore value */
		__asm 
		{
			LDREX   SemaphoreTempValue, [&(pSemaphore->SemaphoreValue)]
		}
		/* Increment semaphore value */
		SemaphoreTempValue++;
		__asm
		{
			STREX  Error ,SemaphoreTempValue,[&pSemaphore->SemaphoreValue]
		}
		if(Error == 0)
		{
			/* Store succeeded */
			terminate = 1;
			__asm
			{
				DMB 
			}
		}
		else
		{
			/* Atomic Operattion Interrupted ... try again */
		}
	}

	if( pSemaphore->WaitingList.numOfItems > 0)
	{
		pTaskToUnblock = pSemaphore->WaitingList.listEnd.pNext->pTask;
		if (pTaskToUnblock == NULL)
		{
			__asm { cpsid i } /* Disable Interrupts */
			while (1)
			{ 			
			}
		}
		List_Remove(&pTaskToUnblock->ObjectListItem);
		if(pTaskToUnblock->StatusListItem.pList != NULL)
		{
			List_Remove(&pTaskToUnblock->StatusListItem);
		}
		AddTaskToReadyList(pTaskToUnblock);
	}
	else
	{
		/* No Tasks are waiting for the semaohore */
	}
}


