/*******************************************************************************
 * File             : codeseg.c
 * Path             : pvm/src
 * Author           : Muhammad Adriano Raksi
 * Created          : 28-09-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the implementation of the VM's code segment.
 ******************************************************************************/

#include "../include/codeseg.h"

int csg_initialise(CodeSeg * codeseg, const char * path)
{
    size_t size;
    FILE * fp;

    fp = fopen(path, "rb");
    if (fp == NULL)
        return pvm_reporterror(CODESEG_H, __FUNCTION__, "File not found");

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    codeseg->size = size;
    codeseg->content = malloc(sizeof(opcode_t) * size + 1);
    fread(codeseg->content, size, 1, fp);
    codeseg->filepath = path;
    fclose(fp);

    return 0;
}

int csg_finalise(CodeSeg * codeseg)
{
    if (codeseg->content != NULL)
        free(codeseg->content);
    codeseg->content = NULL;
    return 0;
}
