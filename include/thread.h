/*******************************************************************************
 * File             : thread.h
 * Path             : pvm/include
 * Author           : Muhammad Adriano Raksi
 * Created          : 04-03-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the interface of the VM's thread. When a thread is spawned, it is
 * given a set of instructions to perform. These instructions are not
 * necessarily the whole bytecode inputted by the user. An instruction performed
 * is called a cycle. Every each cycle, the previously running thread passes
 * the VM program instruction to core in which the core manages which thread to
 * run next.
 ******************************************************************************/

#ifndef THREAD_H
#define THREAD_H 3

#include "scheduler.h"

typedef struct
{
    /*
     * These are the data registers. There are 8 General Purpose Registers (GPR)
     * and 1 Arithmetic Registers (AR). GPR holds both data and address
     * variables whereas AR holds the output of a(n) logic/arithmetic operation.
     */
    PrimitiveData genpreg[8], aritreg;

    /*
     * Program Counter Register holds the number of operations the thread has
     * performed.
     */
    uint64_t progcountreg;

    /*
     * Instruction Pointer Register holds the address variable (index) to the
     * next opcode the thread needs to perform. This is located in the code
     * segment.
     */
    va_t instrpointreg;

    /*
     * Instruction Register holds the opcode the thread is currently performing.
     */
    opcode_t instrreg;
} ControlUnit;

typedef struct _PineVMThread
{
    /* Thread flag */
    uint8_t flag;

     /*
      * Countdown to run again from sleep. This is defined when the thread is
      * passed to thr_Sleep. Countdown is decremented by core_Cycle.
      */
    vmclock_t countdown;

    /* Control Unit is where all the registers are located */
    ControlUnit controlunit;

    /* Stack is thread's private memory */
    Stack stack;
} Thread;

/*
 * Function : thr_spawn
 * --------------------
 * Spawns a thread and initialises its members.
 *
 * @param   : Pointer to VM instance
 * @param   : Address to code segment for the thread's
 *            first operation
 * @return  : Error code
 */
int thr_spawn(VM *, va_t);

/*
 * Function : thr_kill
 * --------------------
 * Kills a thread and frees any dynamically allocated members.
 *
 * @param   : Pointer to VM instance
 * @param   : Thread ID
 * @return  : Error code
 */
int thr_kill(VM *, va_t);

/*
 * Function : thr_run
 * --------------------
 * Lets thread perform a cycle.
 *
 * @param   : Pointer to VM instance
 * @param   : Address to code segment for the thread's first operation
 * @return  : Calls core_Cycle
 */
int thr_run(VM *, va_t);

/*
 * Function : thr_sleep
 * --------------------
 * Sleeps a thread.
 *
 * @param   : Pointer to VM instance
 * @param   : Address to code segment for the thread's first operation
 * @param   : Clocks for thread's countdown
 * @return  : Error code
 */
int thr_sleep(VM *, va_t, vmclock_t);

#define THR_UNINIT  0 /* Thread uninitialised */
#define THR_DEAD    1 /* When a thread is killed */
#define THR_ALIVE   2 /* When a thread is spawned */
#define THR_RUN     4 /* Thread is currenly running */
#define THR_SLEEP   8 /* Thread is not running/waiting for other threads */

#endif /* THREAD_H */
