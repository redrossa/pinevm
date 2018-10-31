/*******************************************************************************
 * File             : static.h
 * Path             : pvm/include
 * Author           : Muhammad Adriano Raksi
 * Created          : 25-04-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the interface of the VM's static segment. The static segment space
 * must be explicitly stated in the bytecode header. The static segment is
 * structured in a way such that the whole segment is primarily a dynamic array.
 * Inside this array is another array. Each static data/variable is made up of
 * the VM's primitive data, since more complex data can created by combining
 * primitive data. The outer array holds all those static data/variables. This
 * segment has a minimum space of 0.
 ******************************************************************************/

#ifndef STATICSEG_H
#define STATICSEG_H 7

#include "common.h"

typedef struct
{
    /* Size of a static variable */
    size_t size;

    /* A static data is made up of an array primitive data */
    PrimitiveData *primdata_arr;
} StaticData;

typedef struct PineVMStaticSegment
{
    /* Size of pool */
    size_t size;

    /* Array of static variables */
    StaticData *var_pool;
} StaticSeg;

/*
 * Function : ssg_initialise
 * -------------------------
 * Initialises static segment. Allocates static pool.
 *
 * @param   : Pointer to StaticSeg instance
 * @param   : Size of static pool to allocate
 * @return  : Error code
 */
int ssg_initialise(StaticSeg *, size_t);

/*
 * Function : ssg_allocate
 * -------------------------
 * Allocates StaticData variable (Pointer to PrimitiveData) in static pool
 *
 * @param   : Pointer to StaticSeg instance
 * @param   : Address in static pool
 * @param   : Size of static variable to allocate
 * @return  : Error code
 */
int ssg_allocate(StaticSeg *, va_t, size_t);

/*
 * Function : ssg_finalise
 * -------------------------
 * Finalises static segment. Frees any allocated StaticData in static pool and
 * finally frees the static pool itself.
 *
 * @param   : Pointer to StaticSeg instance
 * @return  : Error code
 */
int ssg_finalise(StaticSeg *);

#endif /* STATICSEG_H */
