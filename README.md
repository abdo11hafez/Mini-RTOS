# Mini Real Time Operating System
**In this project, I used C-language and ARM Assembly ISA to develop implement a
Preemptive Kernel Based OS**

## the OS specefications:-
- **Kernel Objects Available:**
    - Tasks.
    - Queues.
    - Semaphores.
    - Mutexes.
    
    All Kernel Objects are Created using static memory allocation,i.e. Before Program Execution.
    
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
    
    All Kernel Services (Except OS_Init) are Called via a Supervisor Call Exception (SVC),to Allow those Servics to Access all the
    Proccesor Resourses in the Unprevliged State.    
