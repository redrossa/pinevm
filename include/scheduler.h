/*******************************************************************************
 * File             : scheduler.h
 * Directory        : pvm/include
 * Author           : Muhammad Adriano Raksi
 * Created          : 08-05-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains scheduler structure of the VM and its functions. Scheduler (could
 * also be known as clock) works to keep threads in sync and calculate thread
 * cycle counts.
 ******************************************************************************/

#ifndef SCHEDULER_H
#define SCHEDULER_H 4

#include "stack.h"

typedef unsigned long vmclock_t;

typedef struct PineVMScheduler
{
    /*
     * Holds the scheduler status (SCH_ON/SCH_OFF). @TODO: Currently, the
     * scheduler turned ON when the VM is initialised. It should be turned ON
     * when the master thread is spawned and it should be turned OFF when the
     * master thread dies.
     */
    uint8_t flag;

    /*
     * The total number of thread operations already performed. Note that this
     * is shared by all threads.
     */
    vmclock_t clocks;
} Scheduler;

/*
 * Function : sch_Initialise
 * ------------------------
 * Initialises scheduler.
 *
 * @param   : Pointer to Scheduler instance
 * @return  : Error code
 */
int sch_Initialise(Scheduler *);

/*
 * Function : sch_Pause
 * -------------------
 * Turn scheduler off, cannot increment clock when off.
 *
 * @param   : Pointer to Scheduler instance
 * @return  : Error code
 */
int sch_Pause(Scheduler *);

/*
 * Function : sch_Resume
 * --------------------
 * Turn schedueler when paused.
 *
 * @param   : Pointer to Scheduler instance
 * @return  : Error code
 */
int sch_Resume(Scheduler *);

/*
 * Function : sch_Cycle
 * --------------------
 * A cycle is when a thread has finished performing an operation (including its
 * safety protocol). This increaments scheduler clock.
 *
 * @param   : Pointer to Scheduler instance
 * @return  : Error code
 */
int sch_Cycle(Scheduler *);

/*
 * Function : sch_Cycle
 * --------------------
 * Resets the scheduler clock back to 0.
 *
 * @param   : Pointer to Scheduler instance
 * @return  : Error code
 */
int sch_Reset(Scheduler *);

/*
 * Function: sch_Cycle
 * --------------------
 * Returns scheduler clock to be stored elsewhere in the thread possibly as a
 * data variable.
 *
 * @param   : Pointer to Scheduler instance
 * @return  : Scheduler clock
 */
vmclock_t sch_Stamp(Scheduler *);

/* Scheduler flags */
#define SCH_OFF    0
#define SCH_ON     1

#endif /* SCHEDUELR_H */
