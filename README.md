# Mini Real Time Operating System
**In this project, I used C-language and ARM ISA to develop a
Preemptive Kernel Based OS**

## the OS Specefications:-
- **Kernel Objects Available:**
    - Tasks.
    - Queues.
    - Semaphores.
    - Mutexes.
    
    All Kernel Objects are Created using **static memory allocation**,i.e. Before Program Execution.
    
- **Kernel Services Available:**
    - `OS_Init`: for initializing the SysTick Timer, Exception Priorities and the Ready Tasks List.
    - `OS_StartScheduler`: for Starting the OS.
    - `OS_GetSchedulerStatus`: to determine the Schedular Status (Running or not Running).
    - `OS_TaskCreate`: to Create a Task and add it to the Ready Tasks List.
    - `OS_TaskDelay`: to Remove a Task from the Ready Tasks List for certain amount of Systick Timer Ticks.
    - `OS_SemaphoreCreate`: to Create a Semaphore Object and Assign an Initial Value to it (0:n).
    - `OS_SemaphoreTake`: to Acquire the Semaphore if it's Available.
    - `OS_SemaphoreGive`: to Release the Semaphore.
    - `OS_MutexCreate`: to Create a Mutex Object and Assign an Initial Value to it (0 or 1).
    - `OS_MutexLock`: to Acquire the Semaphore if it's Available.
    - `OS_MutexRelease`: to Release the Semaphore.
    - `OS_QueueCreate`: to Create a Message Queue Object.
    - `OS_QueueWrite`: to Send a Message to a Message Queue.
    - `OS_QueueRead`: to receive a Message from a Message Queue.
    
    All Kernel Services (Except OS_Init) are Called via a **Supervisor Call Exception (SVC)**,to Allow those Servics to Access all the
    Proccesor Resourses in the **Privileged** State.   
    
- **Other Specifications:**
    - Context Switching Operation is Done through a **Pended Service Call Exception (PendSV)** 
      Configured as the lowest Priority Exception to insure that there is no higher priority exceptions (OS Services) are Pending.
      
    - Shared Resourcs Protection Objects(Semaphores, Mutexes and Message Queues) Are **Timer Supported**, Allowing the Task to 
      Wait for a certain amount of SysTick Timer Ticks in case of the Semaphore or Mutex not available for Acquiring 
      or the Message Queues Empty or full in case of Reading or Writing.
      
    - Developed and Tested on **Texas Instruments TM4C123GH6PM LaunchPad**. 
    - Shall Work on **any ARM Cortex M-4 Based Micro-controller** Without any Changes.
    - Developed on Keil uVision4.
    - **No External Liberaries used** (Like CMSIS and TivaWare).
