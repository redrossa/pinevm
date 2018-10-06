/*******************************************************************************
 * File             : scheduler.c
 * Directory        : pvm/src
 * Author           : Muhammad Adriano Raksi
 * Created          : 08-05-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the implementation of the VM's scheduler. Scheduler is simply a
 * unit that acts like a clocks, whereby the number is incremented every cycle
 * of a thread. The core uses scheduler to synchronise the spawned threads such
 * as recalculating cooldowns of any sleeping threads.
 ******************************************************************************/

#include "../include/scheduler.h"

inline int sch_Initialise(Scheduler *scheduler)
{
    scheduler->flag = SCH_ON;
    scheduler->clocks = 0;

    return 0;
}

inline int sch_Pause(Scheduler *scheduler)
{
    if (scheduler->flag == SCH_OFF)
        return pvm_ReportError(SCHEDULER_H, __FUNCTION__, NULL);
    scheduler->flag = SCH_OFF;

    return 0;
}

inline int sch_Resume(Scheduler *scheduler)
{
    if (scheduler->flag == SCH_ON)
        return pvm_ReportError(SCHEDULER_H, __FUNCTION__, NULL);
    scheduler->flag = SCH_ON;

    return 0;
}

inline int sch_Cycle(Scheduler *scheduler)
{
    if (scheduler->flag == SCH_OFF)
        return pvm_ReportError(SCHEDULER_H, __FUNCTION__, NULL);
    scheduler->clocks++;

    return 0;
}

inline int sch_Reset(Scheduler *scheduler)
{
    return sch_Initialise(scheduler);
}

inline vmclock_t sch_Stamp(Scheduler *scheduler)
{
    return scheduler->clocks;
}
