#include <stdint.h>
#include "M4MemMap.h"               
#include "OS.h"
#include "task.h"


extern volatile uint32_t sysTickCounter ;

__asm void PendSV_Handler(void)
{  
 	extern GetRunningTask
	extern UpdateRunningTask
    mrs r1, psp                	/* R1 points to the top of the 8 stacked Regs*/
    mov r2, lr                 	/* so we need to store [R4:R11]+CONTROL+svcException	*/
    mrs r3, control           	/* LR Holds the svcEXC_Return*/
    stmdb r1!,{r2-r11}         	/* save the other 10 Regs and Update R1 to point to thr top of TCB */                                
    bl GetRunningTask       	/* Get the Current Task Stack Pointer (return stoerd in r0) */
    str r1,[r0]                	/* Upadte The Current Task Stak Pointer ([r0]) to Point to
		                           the Top of The New TCB (r1)*/
   
    mov r0, #1                   
    msr basepri, r0             
    bl UpdateRunningTask      
    mov r0, #0                  
    msr basepri, r0
		
    bl GetRunningTask         
    ldr r1,[r0]                  
    ldmia r1!,{r2-r11}                                           
    mov lr, r2                   
    msr control, r3              
    isb  
    msr psp, r1                  
    bx lr                  
	}
 
	