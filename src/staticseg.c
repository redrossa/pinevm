/*******************************************************************************
 * File             : staticseg.c
 * Path             : pvm/src
 * Author           : Muhammad Adriano Raksi
 * Created          : 25-04-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the implementation of the VM's staticsegic segment. User defined data
 * that is inserted into the heap are made up of VM's primitive data. Once
 * defined, variables stored here cannot be freed by the user's will. They
 * are automatically freed by the VM once the user's program ends.
 ******************************************************************************/

#include "../include/staticseg.h"
#include <stdio.h>

int ssg_initialise(StaticSeg *staticseg, FILE * fp)
{
    int type;

    /* Initialise staticseg */
    fread(&staticseg->size, sizeof(uint32_t), 1, fp);
    staticseg->size = REVERSE_32(staticseg->size);
    staticseg->var_pool = malloc(sizeof(StaticData) * staticseg->size);

    /* Traverse to initialise each var_pool */
    for (int i = 0; i < staticseg->size; i++)
    {
        fread(&staticseg->var_pool[i].size, sizeof(uint32_t), 1, fp);
        staticseg->var_pool[i].size = REVERSE_32(staticseg->var_pool[i].size);
        staticseg->var_pool[i].primdata_arr = malloc(sizeof(PrimitiveData) * staticseg->var_pool[i].size);

        /* Traverse to initialise primitive data in a var_pool */
        for (int j = 0; j < staticseg->var_pool[i].size; j++)
        {
            fread(&type, sizeof(uint8_t), 1, fp);
            switch (type)
            {
                case 0:
                    staticseg->var_pool[i].primdata_arr[j].storage = I8;
                    fread(&staticseg->var_pool[i].primdata_arr[j].i8, sizeof(uint8_t), 1, fp);
                    break;
                case 1:
                    staticseg->var_pool[i].primdata_arr[j].storage = UI8;
                    fread(&staticseg->var_pool[i].primdata_arr[j].ui8, sizeof(uint8_t), 1, fp);
                    break;
                case 2:
                    staticseg->var_pool[i].primdata_arr[j].storage = I16;
                    fread(&staticseg->var_pool[i].primdata_arr[j].i16, sizeof(uint16_t), 1, fp);
                    break;
                case 3:
                    staticseg->var_pool[i].primdata_arr[j].storage = UI16;
                    fread(&staticseg->var_pool[i].primdata_arr[j].ui16, sizeof(uint16_t), 1, fp);
                    break;
                case 4:
                    staticseg->var_pool[i].primdata_arr[j].storage = I32;
                    fread(&staticseg->var_pool[i].primdata_arr[j].i32, sizeof(uint32_t), 1, fp);
                    break;
                case 5:
                    staticseg->var_pool[i].primdata_arr[j].storage = UI32;
                    fread(&staticseg->var_pool[i].primdata_arr[j].ui32, sizeof(uint32_t), 1, fp);
                    break;
                case 6:
                    staticseg->var_pool[i].primdata_arr[j].storage = I64;
                    fread(&staticseg->var_pool[i].primdata_arr[j].i64, sizeof(uint64_t), 1, fp);
                    break;
                case 7:
                    staticseg->var_pool[i].primdata_arr[j].storage = UI64;
                    fread(&staticseg->var_pool[i].primdata_arr[j].ui64, sizeof(uint64_t), 1, fp);
                    break;
                case 8:
                    staticseg->var_pool[i].primdata_arr[j].storage = DBL;
                    fread(&staticseg->var_pool[i].primdata_arr[j].dbl, sizeof(uint64_t), 1, fp);
                    break;
                case 9:
                    staticseg->var_pool[i].primdata_arr[j].storage = VA;
                    fread(&staticseg->var_pool[i].primdata_arr[j].va, sizeof(uint64_t), 1, fp);
                    break;
            }
        }
    }

    return 0;
}

int ssg_allocate(StaticSeg *staticseg, va_t va, size_t size)
{
    staticseg->var_pool[va] = (StaticData) {.size = size, .primdata_arr = malloc(sizeof(PrimitiveData) * size)};
    if (staticseg->var_pool[va].primdata_arr == NULL)
        return pvm_reporterror(STATICSEG_H, __FUNCTION__, "Allocation failed");
    return 0;
}

int ssg_finalise(StaticSeg *staticseg)
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
