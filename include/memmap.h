/*******************************************************************************
 * File     : memmap.h
 * Path     : pvm/include
 * Author   : Muhammad Adriano Raksi
 * Created  : 31-10-18 (DD-MM-YY)
 ******************************************************************************/

#include "common.h"
#include <stdlib.h>

/*
 * Memory Layout
 *
 * +-----------+
 * |0          |
 * |           |
 * |   Code    |
 * |           |
 * |          w|
 * +-----------+   +---------+
 * |w+1        |   |w+1  |-->|
 * |  Static   | ->|w+...|-->|
 * |          x|   |w+...| x | (x = w + size of static segment)
 * +-----------+   +---------+
 * |x+1        |
 * |   Stack   |
 * |          y|
 * +-----------+
 * |y+1        |
 * |           |
 * |   Heap    |
 * |           |
 * |          z|
 * +-----------+
 */

typedef struct PineVMMemoryMap
{
    va_t codeseg;
    va_t staticseg;
    va_t stack;
    va_t heap;
} MemMap;
