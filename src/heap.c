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

int heap_initialise(Heap *heap, size_t size)
{
    *heap = (Heap) {.freeframes = size, .totalblocks = 0, .size = size, .var_pool = malloc(sizeof(HeapFrame) * size)};

    if (heap->var_pool == NULL)
        return pvm_reporterror(HEAP_H, __FUNCTION__, "Allocation Failed");

    for (va_t i = 0; i < size; i++)
        heap->var_pool[i] = (HeapFrame) {.free = true, .framesize = 0, .block = NULL};

    /* Allocate for 'null' */
    heap_malloc(heap, 0x0, 1);
    return 0;
}

int heap_finalise(Heap *heap)
{
    if (heap->var_pool != NULL)
    {
        heap->totalblocks = heap->freeframes = 0;

        /* Free any allocated frames first */
        for (va_t i = 0x0; i < heap->size; i++)
            if (heap->var_pool[i].free == false)
                heap_free(heap, i);

        /* Finaly free the frame var_pool */
        free(heap->var_pool);
    }
    heap->var_pool = NULL;

    return 0;
}

int heap_malloc(Heap *heap, va_t va, size_t size)
{
    if (heap->var_pool[va].free == false || heap->freeframes == 0)
        return pvm_reporterror(HEAP_H, __FUNCTION__, "Heap Overflow");

    heap->freeframes--;
    heap->totalblocks += size;

    heap->var_pool[va].free = false;
    heap->var_pool[va].framesize = size;
    heap->var_pool[va].block = malloc(sizeof(PrimitiveData) * size);

    if (heap->var_pool[va].block == NULL)
        return pvm_reporterror(HEAP_H, __FUNCTION__, NULL);

    return 0;
}

int heap_calloc(Heap *heap, va_t va, size_t size)
{
    if (heap->var_pool[va].free == false || heap->freeframes == 0)
        return pvm_reporterror(HEAP_H, __FUNCTION__, "Heap Overflow");

    heap->freeframes--;
    heap->totalblocks += size;

    heap->var_pool[va].free = false;
    heap->var_pool[va].framesize = size;
    heap->var_pool[va].block = calloc(size, sizeof(PrimitiveData));

    if (heap->var_pool[va].block == NULL)
        return pvm_reporterror(HEAP_H, __FUNCTION__, NULL);

    return 0;
}

int heap_realloc(Heap *heap, va_t va, size_t size)
{
    if (heap->var_pool[va].free == true)
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
    if (heap->var_pool[va].free == true)
        return pvm_reporterror(HEAP_H, __FUNCTION__, "Target unallocated");

    heap->freeframes++;
    heap->totalblocks -= heap->var_pool[va].framesize;

    /* Free frame */
    heap->var_pool[va].free = true;
    heap->var_pool[va].framesize = 0;
    free(heap->var_pool[va].block);
    heap->var_pool[va].block = NULL;

    return 0;
}
