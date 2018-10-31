/*******************************************************************************
 * File             : scheduler.c
 * Path             : pvm/src
 * Author           : Muhammad Adriano Raksi
 * Created          : 08-05-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the implementation of the VM's scheduler. Scheduler is simply a
 * unit that acts like a clocks, whereby the number is incremented every cycle
 * of a thread. The core uses scheduler to synchronise the spawned threads such
 * as recalculating cooldowns of any sleeping threads.
 ******************************************************************************/

#include "../include/scheduler.h"

inline int sch_initialise(Scheduler *scheduler)
{
    scheduler->flag = SCH_ON;
    scheduler->clocks = 0;

    return 0;
}

inline int sch_pause(Scheduler *scheduler)
{
    if (scheduler->flag == SCH_OFF)
        return pvm_reporterror(SCHEDULER_H, __FUNCTION__, NULL);
    scheduler->flag = SCH_OFF;

    return 0;
}

inline int sch_resume(Scheduler *scheduler)
{
    if (scheduler->flag == SCH_ON)
        return pvm_reporterror(SCHEDULER_H, __FUNCTION__, NULL);
    scheduler->flag = SCH_ON;

    return 0;
}

inline int sch_cycle(Scheduler *scheduler)
{
    if (scheduler->flag == SCH_OFF)
        return pvm_reporterror(SCHEDULER_H, __FUNCTION__, NULL);
    scheduler->clocks++;

    return 0;
}

inline int sch_reset(Scheduler *scheduler)
{
    return sch_initialise(scheduler);
}

inline vmclock_t sch_stamp(Scheduler *scheduler)
{
    return scheduler->clocks;
}
