#include "Queue.h"
#include "List.h"
#include "Task.h"
#include "M4MemMap.h"
#include <string.h>


void QueueCreate(Queue_t * pQueue, void * pBuffer,uint32_t QueueLength, uint32_t MessageSize)
{
	if ((pQueue==NULL)||(pBuffer==NULL)||(QueueLength==0)||((MessageSize!=1)&&(MessageSize!=2)&&(MessageSize!=4)))
	{
		__asm { CPSID i }         /* Disable Interrupts */
		while (1)
		{ 			
		}
	}

	pQueue->pFront = (int8_t *) pBuffer;
	pQueue->pEnd = pQueue->pFront + (QueueLength * MessageSize);
	pQueue->pReadIndex = pQueue->pFront;
	pQueue->pWriteIndex = pQueue->pFront;
	pQueue->QueueLength = QueueLength;
	pQueue->MessageSize = MessageSize;
	pQueue->MessagesCount = 0;

	List_Init(&pQueue->WaitingList);
}


uint8_t QueueWrite(Queue_t * pQueue, int32_t WaitTicks,const void * const pMessage)
{
	uint8_t returnStatus = 0;
	Task_t *pTask;

	if((pQueue==NULL)||(WaitTicks <-1)||(pMessage==NULL))
	{
		__asm { CPSID i }         /* Disable Interrupts */
		while (1)
		{ 			
		}
	}
	if(pQueue->QueueLength > pQueue->MessagesCount)
	{
		memcpy((void *) pQueue->pWriteIndex, pMessage, pQueue->MessageSize);
		pQueue->pWriteIndex += pQueue->MessageSize;
		if(pQueue->pWriteIndex >= pQueue->pEnd)
		{
			pQueue->pWriteIndex = pQueue->pFront;
		}	
		pQueue->MessagesCount++;
		returnStatus = 1;
		if(pQueue->WaitingList.numOfItems > 0)
		{
			pTask = pQueue->WaitingList.listEnd.pNext->pTask;
			if (pTask==NULL)
			{
				__asm { CPSID i }         /* Disable Interrupts */
				while (1)
				{ 			
				}
			}
			List_Remove(&pTask->ObjectListItem);
			if(pTask->StatusListItem.pList != NULL)
			{
				List_Remove(&pTask->StatusListItem);
			}
			AddTaskToReadyList(pTask);
		}
	}	
	/* Check waiting time and return status */
	if((0 != WaitTicks) && (1 != returnStatus))	
	{
		pTask = GetRunningTask();
		List_Remove(&pTask->StatusListItem);
		List_Insert(&pQueue->WaitingList, &pTask->ObjectListItem);
		SCB_INTCTRL_REG |= 1<<28 ;

		if(0 < WaitTicks)
		{
		DelayRunningTask(WaitTicks);
		}
		else
		{
		}
		returnStatus = 2;
	}
	else
	{
	}
return returnStatus;
}

uint8_t QueueRead(Queue_t * pQueue, int32_t WaitTicks,void * const pMessage)
{
	uint8_t returnStatus = 0;
	Task_t * pTask = NULL;
	if((pQueue==NULL)||(WaitTicks <-1)||(pMessage==NULL))
	{
		__asm { CPSID i }         /* Disable Interrupts */
		while (1)
		{ 			
		}
	}
	/* Check if there are messages in the buffer */
	if( pQueue->MessagesCount > 0)
	{
		memcpy(pMessage, (void *) pQueue->pReadIndex, pQueue->MessageSize);
		pQueue->pReadIndex += pQueue->MessageSize;
		if(pQueue->pReadIndex >= pQueue->pEnd)
		{
		pQueue->pReadIndex = pQueue->pFront;
		}
		else
		{
		}
		pQueue->MessagesCount--;
		returnStatus = 1;
		/* Check waiting threads */
		if(pQueue->WaitingList.numOfItems > 0)
		{
			pTask = pQueue->WaitingList.listEnd.pNext->pTask;
			if (pTask==NULL)
			{
				__asm { CPSID i }         /* Disable Interrupts */
				while (1)
				{ 			
				}
			}
			List_Remove(&pTask->ObjectListItem);
			if(pTask->StatusListItem.pList != NULL)
			{
				List_Remove(&pTask->StatusListItem);
			}
			AddTaskToReadyList(pTask);
		}
	}
	else
	{
	/* Do nothing, Buffer is empty */
	}

	/* Check waiting time and return status */
	if((WaitTicks != 0) && (returnStatus != 1))
	{
		/* Block current thread */
		pTask = GetRunningTask();
		List_Remove(&pTask->StatusListItem);
		List_Insert(&pQueue->WaitingList, &pTask->ObjectListItem);
		SCB_INTCTRL_REG |= 1<<28 ;
		if(0 < WaitTicks)
		{
		DelayRunningTask(WaitTicks);
		}
		else
		{
		/* Infinite Wait */
		}
		returnStatus = 2;
	}
	return returnStatus;
}
