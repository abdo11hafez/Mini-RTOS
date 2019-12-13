#include <stdint.h>
#include "M4MemMap.h"               
#include "OS.h"
#include "task.h"
#include "Semaphore.h"
#include "Mutex.h"
#include "Queue.h"

void OS_SVC_Handler_main(uint32_t * svc_args);

__asm void SVC_Handler(void)
{
	extern svcEXEReturn
	tst lr, #4                	   /* Test bit 2 of LR to find current stack */
	ite eq                    	   /* ITE condition on zero flag(result of TST) */
	mrseq r0, msp             	   /* msp when bit 2 is cleared */
	mrsne r0, psp             	   /* psp when bit 2 is set */
	ldr r1,=svcEXEReturn      	   /* Load the address of svcEXEReturn in r1 */
	str lr,[r1]               	   /* Store the current LR value in svcEXEReturn */
	bl __cpp(OS_SVC_Handler_main)  /* Run C part of SVC Handler */
	ldr r1,=svcEXEReturn      	   /* Load the address of svcEXEReturn in r1 */
	ldr lr,[r1]               	   /* Load LR with svcEXEReturn value 
									(this Value may has been updated by SVC_Handler_main  */
	bx lr                     	   /* Return */
}

void OS_SVC_Handler_main(uint32_t * svc_args)
{
	uint8_t svc_number;
	uint8_t WaitReturn=0;
	/* Memory[(Stacked PC)-2] */
	svc_number = ((char *) svc_args[6])[-2];

	/* Check svc number */
	switch(svc_number)
	{
		case 0:
			Start_Scheduler();
			break;

		case 1:
			Create_Task((Task_t *) svc_args[0],(Stack_t *) svc_args[1],(uint32_t) svc_args[2],(void *) svc_args[3]);
			break;

		case 2:
			SemaphoreCreate((Semaphore_t * ) svc_args[0],(uint32_t) svc_args[1]);
			break;

		case 3:
			WaitReturn = SemaphoreTake((Semaphore_t * ) svc_args[0],(uint32_t) svc_args[1]);
			break;

		case 4:
			SemaphoreGive((Semaphore_t * ) svc_args[0]);
			break;

		case 5:
			CreateMutex((Mutex_t * ) svc_args[0],(uint32_t) svc_args[1]);
			break;

		case 6:
			WaitReturn = MutexLock((Mutex_t * ) svc_args[0],(uint32_t) svc_args[1]);
			break;

		case 7:
			MutexRelease((Mutex_t * ) svc_args[0]);
			break;

		case 8:
			DelayRunningTask((uint32_t) svc_args[0]);
			break;

		case 9:
			QueueCreate((Queue_t *) svc_args[0],(void *)svc_args[1],(uint32_t)svc_args[2],(uint32_t)svc_args[3]);
			break;

		case 10:
			WaitReturn = QueueWrite((Queue_t *) svc_args[0],(int32_t)svc_args[1],(void *)svc_args[2]);
			break;

		case 11:
			WaitReturn = QueueRead((Queue_t *) svc_args[0],(int32_t)svc_args[1],(void *)svc_args[2]);
			break;

		default:
			/* Not supported SVC Number */
			__asm { cpsid i } /* Disable Interrupts */	
			while (1)
			{ 			
			}
			break;
	}
	switch(WaitReturn)
	{
		case 2:
			svc_args[6] = svc_args[6] - 2;
			if((int32_t) svc_args[1] > 0)
			{
				svc_args[1] = 0;
			}
			break;
	}
}





