/*******************************************************************************
 * File             : opcode.h
 * Path             : pvm/include
 * Author           : Muhammad Adriano Raksi
 * Created          : 24-03-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the data type alias of the opcode function type and the array that
 * points to all the opcode functions.
 ******************************************************************************/

#ifndef OPCODE_H
#define OPCODE_H 2

#include "thread.h"

/* Opcode function pointer alias */
typedef opcode_t (*InstructionSet)(VM *, va_t);

/* Opcode function array defined in opcode.c */
extern InstructionSet opc_Execute[256];

#endif /* OPCODE_H */
