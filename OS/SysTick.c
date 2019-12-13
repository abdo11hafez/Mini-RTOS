#include "task.h"
#include "M4memMap.h"

extern volatile uint32_t sysTickCounter ;

uint8_t SysTick_Cnfg(uint32_t ticks)
{
  if ((ticks - 1) > 0xFFFFFFUL)  return (0);        

  SYSTICK_RELOAD_REG  = ticks - 1;                  
  SET_SYSTICK_INT_PRI(7);                          
  SYSTICK_CURRENT_REG = 0x00000000;                 
  SYSTICK_CTRL_REG  |= 0x7 ;                        
  return (1);                                             
}


void SysTick_Handler(void)
{
  /* Trigger context switch (pendSV exception */
  SCB_INTCTRL_REG |= 1<<28 ;
  RefreshTimerList();
}

