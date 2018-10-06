/*******************************************************************************
 * File             : stack.c
 * Directory        : pvm/src
 * Author           : Muhammad Adriano Raksi
 * Created          : 03-03-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the implementation of the VM's stack. Stack is implemented as a LIFO
 * based primdata_array.
 ******************************************************************************/

#include "../include/stack.h"

#define STACK_SIZE 0x400

int stk_Initialise(Stack *stack)
{
    *stack = (Stack) {.pointer = 0, .primdata_arr = malloc(sizeof(PrimitiveData) *STACK_SIZE)};
    return stack->primdata_arr == NULL ? pvm_ReportError(STACK_H, __FUNCTION__, "Allocation failed") : 0;
}

int stk_Finalise(Stack *stack)
{
    free(stack->primdata_arr);
    return 0;
}

stackva_t stk_Push(Stack *stack, PrimitiveData data)
{
    if (stack->pointer == STACK_SIZE)
        pvm_ReportError(STACK_H, __FUNCTION__, "Stack overflow");
    stack->primdata_arr[stack->pointer] = data;
    return ++stack->pointer;
}

stackva_t stk_Pop(Stack *stack)
{
    if (stack->pointer == 0)
        pvm_ReportError(STACK_H, __FUNCTION__, "Stack underflow");
    return --stack->pointer;
}

PrimitiveData stk_Peek(Stack stack)
{
    if (stack.pointer == STACK_SIZE)
        pvm_ReportError(STACK_H, __FUNCTION__, "Stack underflow");
    return stack.primdata_arr[stack.pointer - 1];
}
