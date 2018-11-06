/*******************************************************************************
 * File             : codeseg.c
 * Path             : pvm/src
 * Author           : Muhammad Adriano Raksi
 * Created          : 28-09-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the implementation of the VM's code segment.
 ******************************************************************************/

#include "../include/codeseg.h"

int csg_initialise(CodeSeg * codeseg, const char * path, FILE * fp)
{
    size_t size;
    long int initial_pos;

    /* Find the size of code segment to be allocated */
    initial_pos = ftell(fp);
    fseek(fp, 0, SEEK_END);
    size = ftell(fp) - initial_pos;
    fseek(fp, initial_pos, SEEK_SET);

    /* Initialise code segment */
    codeseg->size = size;
    codeseg->content = malloc(sizeof(opcode_t) * size);
    fread(codeseg->content, sizeof(opcode_t) * size, 1, fp);
    realpath(path, codeseg->filepath);

    return 0;
}

int csg_finalise(CodeSeg * codeseg)
{
    if (codeseg->content != NULL)
        free(codeseg->content);
    codeseg->content = NULL;
    return 0;
}
