# Mini Real Time Operating System
**In this project, I used C-language and ARM Assembly ISA to develop implement a
Preemptive Kernel Based OS**

## the OS specefications:-
- **Kernel Objects Available:**
    - Task.
    - Queues.
    - Semaphores.
    - Mutexes.
    All Kernel Objects are Created using static memory allocation,i.e. Before Program Execution.
    
- **Kernel Services Available:**
    - `OS_Init`: for initializing the SysTick Timer, Exception Priorities and the Ready Tasks List.
    - `OS_StartScheduler`: for Starting the OS.
    - `OS_GetSchedulerStatus`: to determine the Schedular Status (Running or not Running).
    - `OS_TaskCreate`: to Creating a Task and add it to the Ready Tasks List.
    - `OS_TaskDelay`: to Remove a Task from the Ready Tasks List for certain amount of Systick Timer Ticks.
    - `OS_SemaphoreCreate`: to determine the Schedular Status (Running or not Running).
    - `OS_SemaphoreTake`: for initializing the SysTick Timer, Exception Priorities and the Ready Tasks List.
    - `OS_SemaphoreGive`: for Starting the OS.
    - `OS_GetSchedulerStatus`: to determine the Schedular Status (Running or not Running).
    
