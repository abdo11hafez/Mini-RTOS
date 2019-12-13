#include "task.h"
#include "list.h"
#include "OS.h"
#include "M4MemMap.h"

static List_t readyList[TASK_PRIORITY_LEVELS];
static List_t TimerWaitingList;
static uint32_t currentTopPriority = (TASK_PRIORITY_LEVELS - 1);
extern volatile uint64_t sysTickCounter;
Task_t * pRunningTask ;

void Init_Lists(void)
{
	uint32_t priority ;
	for(priority = 0; priority < TASK_PRIORITY_LEVELS; priority++)
	{
		List_Init(&readyList[priority]);
	}
}

void AddTaskToReadyList(Task_t * pTasK)
{
	
	List_Insert(&readyList[pTasK->priority], &pTasK->StatusListItem);  /* Add new Task to ready list */
	if(pTasK->priority < currentTopPriority)   
	{
	currentTopPriority = pTasK->priority;   /* Set current top priority */
	}
	if((NULL != pRunningTask)&& (pTasK->priority < pRunningTask->priority))
	{
		/* Trigger context switch, (pendSV exception) */
		SCB_INTCTRL_REG |= 1<<28 ;
	}
	else
	{
		/* Context switch is not Needed */
	}
}


void Create_Task(Task_t * Task, Stack_t * Stack,uint32_t priority, void * Function)
{
	if ((Task == NULL)||(Stack == NULL)||(priority > (TASK_PRIORITY_LEVELS-1))||(Function == NULL))
	{
		__asm { cpsid i } /* Disable Interrupts */
		while (1)
		{ 			
		}
	}
	/* Save Space For the Task Control Block at Bottom of the Stack, 
	 * TASK_STACK_SIZE multiplied with 4 to convert word length to Byte.
	 * TCB  is 18 words (svcEXC_Return + CONTROL REG + [R0:12] + LR + PC + xPSR )
	*/

	/* Moving the stack pointer to point to the top of stack
	* After reserving Memory for TCB */
	Task->pStack = ((uint32_t)Stack + TASK_STACK_SIZE * 4 - 18 * 4);
	/* Save initial xPSR Value,(THUMB state)*/
	MEM32_ADDRESS((Task->pStack + (17 << 2))) = 0x01000000;
	/* Save the Task function Address into PC of TCB */
	MEM32_ADDRESS((Task->pStack + (16 << 2))) = (uint32_t) Function;
	/* Write EXC_RETURN, since the execution threads are using PSP, this will
	* allow SVC to return to the thread with PSP */
	MEM32_ADDRESS(Task->pStack) = 0xFFFFFFFDUL;
	/* Write initial CONTROL register value UNPRIVILEGED, PSP */
	MEM32_ADDRESS((Task->pStack + (1 << 2))) = 0x3;

	Task->priority = priority;              
	Task->OriginalPriority = priority;
	Task->StatusListItem.pList = NULL;                
	Task->ObjectListItem.pList = NULL;
	Task->StatusListItem.pTask = (void *) Task;       	
	Task->ObjectListItem.pTask = (void *) Task;
	Task->StatusListItem.ItemValue = priority;

	AddTaskToReadyList(Task);

}


void UpdateRunningTask(void)
{
	List_t * pReadyTasks;
	while(readyList[currentTopPriority].numOfItems == 0 )
	{
		if ((currentTopPriority > TASK_PRIORITY_LEVELS-1))
		{
			__asm { cpsid i } /* Disable Interrupts */
			while (1)
			{ 			
			}
		}
		currentTopPriority++;
	}
	
	/* Task are found, update the index to point the next Task */
	pReadyTasks = &readyList[currentTopPriority];
	pReadyTasks->pIndex = pReadyTasks->pIndex->pNext;
	/* Check if the new index pointing to the end of the list */
	if(pReadyTasks->pIndex == (ListItem_t *) &pReadyTasks->listEnd)
	{
		/* Point to the next Task */
		pReadyTasks->pIndex = pReadyTasks->pIndex->pNext;
	}
	else
	{
	}
	/* Update current running thread */
	pRunningTask = (Task_t *) pReadyTasks->pIndex->pTask;
}

Task_t * GetRunningTask(void)
{
  return pRunningTask;
}

void DelayRunningTask(uint32_t WaitTime)
{
	uint32_t WakeUpTime = 0;
	if (WaitTime <= 0)
	{
		__asm { cpsid i } /* Disable Interrupts */
		while (1)
		{ 			
		}
	}
	WakeUpTime = sysTickCounter + WaitTime;
	if(sysTickCounter > WakeUpTime)
	{
		/* TODO: Handle overflow */
		__asm { cpsid i } /* Disable Interrupts */
		while (1)
		{ 			
		}
	}
	pRunningTask->StatusListItem.ItemValue = WakeUpTime;
	List_Remove(&pRunningTask->StatusListItem);
	ListInsertAtItemValue(&TimerWaitingList, &pRunningTask->StatusListItem);
	SCB_INTCTRL_REG |= 1<<28 ;
}


void RefreshTimerList(void)
{
	Task_t * pTask;
	if(OS_GetSchedulerStatus() == 1)
	{
		++sysTickCounter;
		if(0 == sysTickCounter)
		{
			/* counter overflow */
			__asm { cpsid i } /* Disable Interrupts */
			while (1)
			{ 			
			}
		}
		/* Check if timer list has threads waiting */
		if(TimerWaitingList.numOfItems > 0)
		{
			while(sysTickCounter >= TimerWaitingList.listEnd.pNext->ItemValue)
			{
				/* Get first thread waiting */
				pTask = TimerWaitingList.listEnd.pNext->pTask;
				if(pTask == NULL)
				{
					__asm { cpsid i } /* Disable Interrupts */
					while (1)
					{ 			
					}
				}			
				List_Remove(&pTask->StatusListItem);
				if(NULL != pTask->ObjectListItem.pList)
				{
				List_Remove(&pTask->ObjectListItem);
				}
				AddTaskToReadyList(pTask);
			}
		}
		else
		{
			/* Timer Waiting list is empty */
		}
	}
	else
	{
		/* Scheduler is not running */
	}
}

void InheritTaskPrioroty(Task_t *pMutexHolderTask)
{
	if(pMutexHolderTask == NULL)
	{
		__asm { cpsid i } /* Disable Interrupts */
		while (1)
		{ 			
		}
	}
	if (pRunningTask->priority < pMutexHolderTask->priority)
	{
		/* the running task has higher priority than tha mutex holder task */
		/* update the mutex holder task priority */
		pMutexHolderTask->ObjectListItem.ItemValue = pRunningTask->priority;
		if(pMutexHolderTask->StatusListItem.pList == &readyList[pMutexHolderTask->priority])
		{
			List_Remove(&pMutexHolderTask->StatusListItem);
			pMutexHolderTask->priority = pRunningTask->priority;
			AddTaskToReadyList(pMutexHolderTask);
		}
		else
		{
			pMutexHolderTask->priority = pRunningTask->priority;
		}		
	}
}

void DisinheritTaskPriority(void)
{
	if(pRunningTask->priority != pRunningTask->OriginalPriority)
	{
		List_Remove(&pRunningTask->StatusListItem);
		pRunningTask->priority = pRunningTask->OriginalPriority;
		pRunningTask->ObjectListItem.ItemValue = pRunningTask->OriginalPriority;
		AddTaskToReadyList(pRunningTask);
	}
	else
	{
		/* Do nothing */
	}
}







