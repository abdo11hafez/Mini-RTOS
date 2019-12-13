#include "task.h"
#include "Semaphore.h"
#include "OS.h"
#include "GPIO.h"
#include "UART.h"
#include "Mutex.h"
#include "Queue.h"


static Task_t Task1;
static Stack_t Task1stack;

static Task_t Task2;
static Stack_t Task2stack;

static Task_t Task3;
static Stack_t Task3stack;

static Mutex_t Mutex1;
static Semaphore_t Semaphore1;

static Queue_t Queue1;
static uint32_t buff[2];
void Red_LED(void)
{
	int var;
  while(1)
  {
		OS_MutexLock(&Mutex1,-1);
		UART_Send_String(0,"Hello From Task RED\r\n");
    GPIO_Write(0,1<<1);
		OS_MutexRelease(&Mutex1);
		for ( var = 0; var < 500000; ++var){}
		OS_TaskDelay(1);
  }
}
void Blue_LED(void)
{
	int var;
	OS_MutexLock(&Mutex1,-1);
  while(1)
  {
		OS_MutexLock(&Mutex1,5);
		UART_Send_String(0,"Hello From Task Blue\r\n");
    GPIO_Write(0,1<<2);
		OS_MutexRelease(&Mutex1);
		for ( var = 0; var < 500000; ++var){}
		OS_TaskDelay(1);
  }
}

void Green_LED(void)
{
  while(1)
  {
    GPIO_Write(0,1<<3);
		OS_TaskDelay(1);
  }
}
int main(void)
{
	GPIO_Init();
	UART_Init();
	OS_Init();
	
	OS_TaskCreate(&Task2, &Task2stack, 1, Blue_LED);
	OS_TaskCreate(&Task1, &Task1stack, 1, Red_LED);
	OS_TaskCreate(&Task3, &Task3stack, 3, Green_LED);
	OS_MutexCreate(&Mutex1,1);

	
	OS_Start_Scheduler();
	while(1)
	{
		
	}
}

