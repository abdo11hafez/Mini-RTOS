#include "task.h"
#include "M4MemMap.h"
#include "SysTick.h"
#include "Mutex.h"
#include "Semaphore.h"
#include "Queue.h"

volatile uint64_t sysTickCounter = 0;
static uint32_t SchedulerStatus = 0;
uint32_t svcEXEReturn;

extern Task_t * pRunningTask;
static Task_t IdleTask;
static Stack_t IdleTaskStack;

__asm void OS_StartScheduler(void)
{
	svc  0
	bx  LR
}

__asm void OS_TaskCreate(Task_t * Task, Stack_t * Stack,uint32_t priority, void * Function)
{
	svc  1
	bx  LR
}

__asm void OS_SemaphoreCreate(Semaphore_t * pSemaphore, uint32_t InitialValue)
{
	svc  2
	bx  LR
}

__asm uint32_t OS_SemaphoreTake(Semaphore_t * pSemaphore, int32_t WaitTicks)
{
	svc  3
	bx  LR
}

__asm void OS_SemaphoreGive(Semaphore_t * pSemaphore)
{
	svc  4
	bx  LR
}
__asm void OS_MutexCreate(Mutex_t * pMutex ,uint32_t initialValue)
{
	svc  5
	bx  LR
}
__asm void OS_MutexLock(Mutex_t * pMutex,int32_t WaitTicks)
{
	svc  6
	bx  LR
}
__asm void OS_MutexRelease(Mutex_t * pMutex)
{
	svc  7
	bx  LR
}

__asm void OS_TaskDelay(uint32_t Ticks)
{
	svc  8
	bx  LR
}

__asm void OS_QueueCreate(Queue_t * pQueue, void * pBuffer,uint32_t qQueueLength, uint32_t MessageSize)
{
	svc  9
	bx  LR
}

__asm void OS_QueueWrite(Queue_t * pQueue, int32_t WaitTicks,const void * const pMessage)
{
	svc  10
	bx  LR
}
__asm void OS_QueueRead(Queue_t * pQueue, int32_t WaitTicks,void * const pMessage)
{
	svc  11
	bx  LR
}



static void IdleFunction(void)
{
  while(1)
  {

  }
}

void OS_Init(void)
{
	__asm { cpsid i }               /* Disable interrupts */
	SCB_CFGCTRL_REG &=~ (1<<9);     /* Enable word stack alignment */
	if (SysTick_Cnfg(16000000 /1) == 0)
	{
		while (1)
		{ 			
		}
	}
	SET_SVC_INT_PRI(0);           /* Set SVCcalls to Highest Priority */
	SET_SYSTICK_INT_PRI(1);
	SET_PENDSV_INT_PRI(7);        /* Set PendSV to lowest priority(7) */
	__asm                         
	{ 
		MOV  R0,#1                /* Disable all interrupts except SVC */
		MSR  BASEPRI,R0
	}		 
	Init_Lists();                /* Initialize Ready Tasks List */
	__asm { CPSIE  i }           /* Enable interrupts */
}


void StartScheduler(void)
{
	Task_t * pTaskToRun;              
	uint32_t PSP_Value ;
	uint32_t CONTROL_Value;
		
	Create_Task(&IdleTask,&IdleTaskStack,(TASK_PRIORITY_LEVELS - 1),IdleFunction); /* Create idle Task */
	UpdateRunningTask();                                                           /* Update the running Task */
	pTaskToRun = GetRunningTask();                                                 /* Get running Task */
	svcEXEReturn = MEM32_ADDRESS(pTaskToRun->pStack);                              /* Update the Excption Return Value*/

	PSP_Value = (uint32_t)(pTaskToRun->pStack + 10 * 4);
	CONTROL_Value = MEM32_ADDRESS( (pTaskToRun->pStack+(1 << 2)) );
	__asm
	{ 
		MSR PSP , PSP_Value				/* Update PSP Value */
		MSR CONTROL , CONTROL_Value 	/* Switch to use PSP, unprivileged ,Thread Mode */
	    ISB                             /* These Values to Be Used Upon Exception Return */	
	}	
	sysTickCounter = 0;
	SchedulerStatus = 1;
	__asm                         
	{ 
	MOV  R0,#0                     		/* Enable all interrupts */
	MSR  BASEPRI,R0 
	}
}

uint32_t OS_GetSchedulerStatus(void)
{
  return SchedulerStatus;
}






