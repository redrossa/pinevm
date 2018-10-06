/*******************************************************************************
 * File             : staticseg.c
 * Directory        : pvm/src
 * Author           : Muhammad Adriano Raksi
 * Created          : 25-04-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the implementation of the VM's staticsegic segment. User defined data
 * that is inserted into the heap are made up of VM's primitive data. Once
 * defined, variables stored here cannot be freed by the user's will. They
 * are automatically freed by the VM once the user's program ends.
 ******************************************************************************/

#include "../include/staticseg.h"

int ssg_Initialise(StaticSeg *staticseg, size_t size)
{
    *staticseg = (StaticSeg) {.size = size, .var_pool = malloc(sizeof(PrimitiveData) * size)};

    for (va_t i = 0; i < staticseg->size; i++)
        staticseg->var_pool[i] = (StaticData) {.size = 0, .primdata_arr = NULL};

    if (staticseg->var_pool == NULL)
        return pvm_ReportError(STATICSEG_H, __FUNCTION__, "Allocation failed");
    return 0;
}

int ssg_Allocate(StaticSeg *staticseg, va_t va, size_t size)
{
    staticseg->var_pool[va] = (StaticData) {.size = size, .primdata_arr = malloc(sizeof(PrimitiveData) * size)};
    if (staticseg->var_pool[va].primdata_arr == NULL)
        return pvm_ReportError(STATICSEG_H, __FUNCTION__, "Allocation failed");
    return 0;
}

int ssg_Finalise(StaticSeg *staticseg)
{
    if (staticseg->var_pool != NULL)
    {
        for (va_t i = 0; i < staticseg->size; i++)
            if (staticseg->var_pool[i].size > 0)
                free(staticseg->var_pool[i].primdata_arr);
        free(staticseg->var_pool);
    }
    staticseg->var_pool = NULL;
    return 0;
}
