/*******************************************************************************
 * File             : heap.c
 * Path             : pvm/src
 * Author           : Muhammad Adriano Raksi
 * Created          : 26-03-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the implementation of the VM's heap. The heap is implemented as a
 * superset of the static segment. User has full control over data inserted here
 * such as freeing at will and enlarging/shrinking size.
 ******************************************************************************/

#include "../include/heap.h"
#include <string.h>

int heap_initialise(Heap *heap, FILE *fp)
{
    size_t size;
    fread(&size, sizeof(uint32_t), 1, fp);
    size = REVERSE_32(size);
    heap->freeframes = heap->size = size;
    heap->totalblocks = 0;
    heap->var_pool = malloc(sizeof(HeapFrame) * size);

    if (heap->var_pool == NULL)
        return pvm_reporterror(HEAP_H, __FUNCTION__, "Allocation Failed");

    (*heap->var_pool) = (HeapFrame) {.occupied = false, .framesize = 0, .block = NULL};

    return 0;
}

/*
 * It is very important that users free whatever is explicitly allocated
 * because this function does not check for allocated blocks.
 */
int heap_finalise(Heap *heap)
{
    if (heap->size == 0)
        return 0;
    heap->totalblocks = heap->freeframes = 0;
    free(heap->var_pool);

    return 0;
}

int heap_malloc(Heap *heap, va_t va, size_t size)
{
    if (heap->var_pool[va].occupied == true || heap->freeframes == 0)
        return pvm_reporterror(HEAP_H, __FUNCTION__, "Heap Overflow");

    heap->freeframes--;
    heap->totalblocks += size;

    heap->var_pool[va].occupied = true;
    heap->var_pool[va].framesize = size;
    heap->var_pool[va].block = malloc(sizeof(PrimitiveData) * size);

    if (heap->var_pool[va].block == NULL)
        return pvm_reporterror(HEAP_H, __FUNCTION__, NULL);

    return 0;
}

int heap_calloc(Heap *heap, va_t va, size_t size)
{
    if (heap->var_pool[va].occupied == true || heap->freeframes == 0)
        return pvm_reporterror(HEAP_H, __FUNCTION__, "Heap Overflow");

    heap->freeframes--;
    heap->totalblocks += size;

    heap->var_pool[va].occupied = true;
    heap->var_pool[va].framesize = size;
    heap->var_pool[va].block = calloc(size, sizeof(PrimitiveData));

    if (heap->var_pool[va].block == NULL)
        return pvm_reporterror(HEAP_H, __FUNCTION__, NULL);

    return 0;
}

int heap_realloc(Heap *heap, va_t va, size_t size)
{
    if (heap->var_pool[va].occupied == false)
        return pvm_reporterror(HEAP_H, __FUNCTION__, "Target unallocated");

    PrimitiveData * tmp;

    heap->totalblocks -= heap->var_pool[va].framesize; // Minus previous size
    heap->totalblocks += heap->var_pool[va].framesize = size; // New size

    tmp = realloc(heap->var_pool[va].block, size);
    if (tmp == NULL)
        return pvm_reporterror(HEAP_H, __FUNCTION__, "Allocation Failed");

    heap->var_pool[va].block = tmp;
    return 0;
}

int heap_free(Heap *heap, va_t va)
{
    if (heap->var_pool[va].occupied == false)
        return pvm_reporterror(HEAP_H, __FUNCTION__, "Target unallocated");

    heap->freeframes++;
    heap->totalblocks -= heap->var_pool[va].framesize;

    /* Free frame */
    heap->var_pool[va].occupied = false;
    heap->var_pool[va].framesize = 0;
    free(heap->var_pool[va].block);
    heap->var_pool[va].block = NULL;


    return 0;
}
